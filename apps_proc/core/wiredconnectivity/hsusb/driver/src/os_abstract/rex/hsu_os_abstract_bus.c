/*==============================================================================

                 H S U _ O S _ A B S T R A C T _ B U S . C


GENERAL DESCRIPTION
  High-Speed USB Implementation of the REX OS abstraction layer 
  bus API for the REX OS.

EXTERNALIZED FUNCTIONS
  Externalized to outside of the OS abstraction layer:
    hsu_os_interrupt_setup
    hsu_os_interrupt_teardown
    hsu_os_bus_start_stop_lpm_timer
    hsu_os_notify
    hsu_os_hw_init
    hsu_os_hw_uninit
    hsu_os_bus_alloc_resource
    hsu_os_bus_release_resource
    hsu_os_bus_space_read_1
    hsu_os_bus_space_read_2
    hsu_os_bus_space_read_4
    hsu_os_bus_space_write_1
    hsu_os_bus_space_write_2
    hsu_os_bus_space_write_4

    hsu_os_bus_poll_interrupt
    hsu_os_bus_handle_interrupt
    hsu_os_notify_stack_ready

    hsu_os_bus_get_num_cores
    hsu_os_bus_set_lpm_feature_is_active

    hsu_os_bus_lpm_enable
    hsu_os_bus_lpm_disable

    hsu_port_is_attached

    hsu_os_power_request
    hsu_os_get_port_info

    hsu_os_bus_interrupt_init
    hsu_os_bus_interrupt_uninit

  Externalized internally in the OS abstraction layer:
    hsu_os_bus_init
    hsu_os_bus_uninit

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_os_init should be called before using the API defined in this file.

Copyright (c) 2007-2010, 2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR MODULE



  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/20   cs      disabled 166 interrupt line and enabled 168 line to recieve interrupts from QGIC for twizy.
06/08/10   gs      Prepended hsu to function names to prevent improper calls
01/08/06   ds      Created.

===========================================================================*/


/*==============================================================================

                               LOCAL LINT FLAGS

==============================================================================*/

/*lint -emacro(717, ASSERT_CONTEXT)   This macro is defined by 3rd party. */

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

/* AMSS include files. */

#include <comdef.h>
#include <customer.h>

//#include <rex.h>  /* For rex_sleep */
#include "hsu_qurt_util.h"

#include <sleep.h>
#include <event.h>
#include <hsu_phy_utils_int.h>  /* For hsu_phy_utils_spoof_connect\disconnect. */

/* High Speed USB subsystem include files. */
#include <jerrno.h>
#include <jtypes_int.h>
#include <hsu_al_task.h>
#include <hsu_common_int.h>
#include <hsu_core_config_int.h>
#include <controllers.h>

#include <port_sync_int.h>
#include <port_bus_int.h>

#include "hsu_core_config.h"

#include <hsu_conf_sel_stack_utils_int.h>
#include <hsu_event_defs.h>

#include <hsu_config_selector.h>
#include "hsu_conf_sel_stack_utils.h"

#include "hsu_bus_mode_notifier.h"

/* Local include files. */
#include "hsu_os_abstract_bus.h"
#include "hsu_os_abstract_bus_ext_int.h"

#include "hsu_apis.h"

#if defined (FEATURE_HS_USB_OTG) && !defined(FEATURE_HSU_TEST)
#include "hsu_otg_app.h"
#endif /*FEATURE_HS_USB_OTG*/

#define HSU_MAX_PHCD_DWELL_TIME_ATTEMPT  100

#ifndef FEATURE_NEW_SLEEP_API
#error  FEATURE_NEW_SLEEP_API must be defined for HS-USB to vote for sleep correctly
#endif  /*FEATURE_NEW_SLEEP_API*/
#include "HAL_hsusb.h"

#include "hsu_os_abstract_bus_ext_int.h"

#ifdef FEATURE_HS_USB_POLLING
#include "hsu_polling.h"
#endif /* FEATURE_HS_USB_POLLING */

//#include "cache_mmu.h"

#ifdef FEATURE_HS_USB_OS_DETECTION
#include "hsu_os_detection.h"
#endif /* FEATURE_HS_USB_OS_DETECTION */
#include "hsu_conf_sel_types_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_conf_sel_i.h"

#include "hsu_lpm.h"
#include "hsu_lpm_snps.h"

#include "hsu_clkmgr.h"
#include "hsu_vregmgr.h"

#include "hsu_log.h"
#include "hsu_conf_sel_i.h"

/* GPIO APIs */
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "jusb_core.h"
#include "DDIRPM.h"
#include "RPMTypes.h"
#include "hsu_platform.h"

#include "HALhwio.h"
#include "hsu_al_bam_utils.h"
#include "hsu_hwio_dependency.h"

/* Interrupt Controller APIs */
#include "DDIInterruptController.h"
#include "DALStdErr.h" 

/* For QDSS markers */
#include "tracer.h"

#ifndef USB_THREADX
#ifndef FEATURE_HS_USB_PRE_SILICON_BU
#include "tracer_event_ids.h"
#endif // FEATURE_HS_USB_PRE_SILICON_BU
#endif

#include "hsu_timer.h"
#include "hsu_bam_sm.h"

#include "qurt_interrupt.h"
#include "dsr.h"

#define TRAMP_USB_HS_IRQ    166
#define TRAMP_USB_HSIC_IRQ  168

#define TRAMP_USB_HS_WAKEUP_IRQ    172
#define TRAMP_USB_HSIC_WAKEUP_IRQ  180

#define TRAMP_USB3_EE1_IRQ         75
#define TRAMP_USB3_EE2_IRQ         166
#define TRAMP_USB3_POWER_EVENT_IRQ 74

#define TRAMP_USB_HS_PHY_IRQ       166
#define TRAMP_USB_QUSB2_PHY_IRQ    168
#define TRAMP_USB_QMP_PCIE_USB3_LFPS_RXTERM_CX_IRQ 235
/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/* The following defines the memory areas that will be saved when ASSERTed*/
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
#define OTG_REG_MEM_SIZE  512 /*We want to read from BASE_ADDRESS 
                                  to BASE_ADDRESS+1ffh*/
  #ifdef FEATURE_HS_USB_DUMP_USB_PHY_REG
  #define ULPI_REG_MEM_SIZE 64  /*We want to read 0 - 3Fh*/
  #endif
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */

/* minimum delay for remote wake up */
#define OS_REMOTE_WAKEUP_MIN_DELAY_MS (10)

#ifdef FEATURE_HS_USB_POLLING
#define HSU_MSM_SET( base, field ) \
    HWIO_OUTM( base, HWIO_FMSK(base,field), HWIO_FMSK(base,field))
#endif /* FEATURE_HS_USB_POLLING */

/*==============================================================================
Forward declarations
==============================================================================*/
#ifdef FEATURE_HS_USB_F3
#ifdef CI_MERGE
  static hsu_gpio_handle_ctx_type hsu_gpio_hsic_ready_ctx = 
  {
    /* h_tlmm         */  NULL,
    /* h_gpio_int     */  NULL,
    /* pin_config     */  0,
  };
  void hsu_os_ap2mdm_status_gpio_register(void *param);
#endif
  void hsu_os_vddmin_gpio_register(void *param);
  #ifdef FEATURE_HS_USB_VDDMIN_LOG
/* Support for VDD MIN logging */
  static hsu_gpio_handle_ctx_type hsu_apq2mdm_gpio_intr_ctx =
  {
    /* h_tlmm         */  NULL,
    /* h_gpio_int     */  NULL,
    /* pin_config     */  0,
  };
  static boolean vdd_min_gpio_high = FALSE;
  #endif /* FEATURE_HS_USB_VDDMIN_LOG */
#endif /* FEATURE_HS_USB_F3 */

/*==============================================================================
  Typedefs
==============================================================================*/
#define HSU_MDM_VDD_MIN_MODE 0x00000300
#define HSU_MDM_VDD_MIN_DRIVE 0x00000008


/* 
 * Resource IDs for OTG, Device and Host controllers. 
 * See the porting guide document, hsu_os_bus_alloc_resource() API (section 21.3)
 * "id" field description.
 */
enum 
{
  OS_BUS_OTG_RESOURCE_ID    = 0,
  OS_BUS_DEVICE_RESOURCE_ID = 1,
  OS_BUS_HOST_RESOURCE_ID   = 2,
  OS_BUS_NUM_OF_RESOURCE_IDS
};

enum  
{
  OS_BUS_MAX_CONTROLLERS_PER_CORE = 3,
    /* Possible controllers: OTG, Device, Host */

  OS_BUS_MAX_ISRS_PER_INTERRUPT   = 2,
    /* Interrupt handler chaining support. */

  OS_BUS_NUM_RES_PER_CONTROLLER   = 2
};

/* 
 * Defined the core type
 */
typedef enum
{
  OS_BUS_USB_HIGH_SPEED_CORE,
  OS_BUS_USB_FULL_SPEED_CORE,
  OS_BUS_USB_UICC_CORE,
  OS_BUS_USB_HSIC_CORE,
  OS_BUS_USB_USB3_CORE,
} hsusb_core_type;


/*lint -esym(754, length)  
** The length field of a memory resource (memory_resource_info_type) 
** is accessed by HSU_ASSERT macros, but for some reason Lint thinks
** that it is not referenced.
*/

/* I/O or Memory resource info type. */
typedef struct 
{
  uint32           core_index;
  juint8_t* const  start_address;
  const juint32_t  length;  
} memory_resource_info_type;



typedef union
{
  memory_resource_info_type     mem_resource_instance;
  interrupt_resource_info_type  int_resource_instance;
} resource_info_type;

/* High-Speed USB controller resource info type. */
typedef struct 
{
  jint_t                 resource_type;
  jint_t                 resource_id;
  resource_info_type     resource_instance;
} bus_resource_type;

/* High-Speed USB controller info type. */
typedef struct 
{
  jint_t                controller_type;

  /* The resource handle returned by add_generic_controller.
  ** A NULL value implies that the controller is 
  ** not registered with the stack.
  */
  void*                 controller_handle;

  bus_resource_type  resource_list[OS_BUS_NUM_RES_PER_CONTROLLER];

} usb_controller_info_type;

/* USB core information type. */
typedef struct 
{
  char*              description;
  hsusb_core_type    core_type;
  uint32             number_of_controllers;

  usb_controller_info_type  
    controllers_list[OS_BUS_MAX_CONTROLLERS_PER_CORE];

  /* Core's USB interrupt number */
  uint32  interrupt_number;
  
  /* Core's main interrupt handler. */
  void (*interrupt_handler)(void);

  /* Core's Low power mode information */
  hsu_lpm_info_type
    usb_lpm_info;

  /* Core Hardware Initialization Function Pointer part of core table*/
  /* Initialize hsu clock manager and LPM for each core */
void (*core_hw_init)(void);

  /* This timer is armed when the core exits from low-power mode
  ** due to an asynchronous interrupt. When this happens, if we are in host mode
  ** resume all of the connected peripherals sequence is scheduled. 
  ** This operation is delayed (via timer) in order to give the hs-usb0
  ** task enough time to update the cable connection status. 
  ** See documentation of hsu_os_bus_resume_peripherals() for more details.
  */
  hsu_timer_handle_type
    resume_peripherals_timer;

  uint32  num_of_registered_isrs;

  /* An array of the core's registered interrupt service routines
  ** chained by the core's main interrupt handler.
  */
  interrupt_resource_info_type*
    isr_list[OS_BUS_MAX_ISRS_PER_INTERRUPT];

  /* Core's attached controller */
  usb_controller_info_type* controller_info_ptr;

  /*lpm dsr object */
  dsr_obj_t      *hsu_lpm_dsr;

  /* dsr object */
  dsr_obj_t      *hsu_dsr;

} usb_core_info_type;

typedef struct 
{
  uint32 
    spoofed_reconnects_on_reset_during_suspend_count;
  uint32
    phy_access_fail_count;
  uint32
    stuck_phy_resets_count;
} hsu_os_bus_statistics_type;

/*lint -e{754}
** This structure serves also for debugging purposes.
** Possibly not all of the fields are referenced 
** directly by the code.
*/
typedef struct 
{
  usb_core_info_type* const
    core_table_ptr;

  /* Map between core index used in hsu_os_abstract_bus, to HAL or 
     core_config core index */
  uint8    *hal_core_index_map_ptr;

  /* Map between core index used in HAL or core_config, to hsu_os_abstract_bus
     core index */
  uint8    *hal_core_index_inv_map_ptr;

  /* Statistic info collected for debugging purpose. */
  hsu_os_bus_statistics_type*
    hsu_os_bus_statistics_ptr;

  /* This handle is used for VREG voting at the PMIC. */
// 9x25 BU
  /* pm_vote_vreg_app_type
    hs_usb_vreg_3p3v_vote_handle; */

  /* This flag enables\disables the low-power mode feature. */
  boolean
    lpm_feature_is_active;

  /* If this flag is TRUE and PHY access fails, it will be reset */
  boolean 
    reset_phy_enabled;

  /* The usbh_pclk control feature is not enabled for the U2 target, because
  ** this feature works fine only if the ULPI_STP_CTRL bit in the USBCMD 
  ** register is set upon LPM entry and cleared in LPM exit. However, doing this
  ** causes problems in the USB resume sequence during TCXO shutdown on the U2 
  ** target.
  */
  /* This flag enables\disables the usbh_pclk clock control feature. */
  boolean
    usbh_pclk_control_active;

  /* This flag enables\disables the ULPI GPIO config\unconfig feature. */
  boolean
    gpio_control_active;

  /* This flag enables\disables the CPU clock control feature. */
  boolean
    dcvs_active;

  /* This critical section is used for maintaining mutual-exclusion 
  ** between the low-power mode entry and exit functions.
  */

  rex_crit_sect_type
    lpm_crit_sect;

  #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
  /* Indicates whether or not the PMIC MPP15 - GPIO 111 rework for supporting 
  USB reset waking device from TCXO shutdown exists. */
  boolean usb_reset_rework_is_installed;
  #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
  /* The following memory is used to saved the relevant USB registers
  ** in case of an ASSERT. It used for main core only
  */
  uint32   *otg_controller_mem_dump_ptr;

  #ifdef FEATURE_HS_USB_DUMP_USB_PHY_REG
    uint8    *ulpi_reg_mem_dump_ptr;
  #endif
#endif  /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */



  /* remote wakeup delay time in msec */
  timetick_type  remote_wakeup_delay;
} os_bus_file_static_vars_type;

/*==============================================================================
  Constants and Macros
==============================================================================*/

#ifdef FEATURE_HS_USB_SNPS_CORE
/* SNPS controller memory mapped section - base address and length */
#define OS_BUS_XDCI_START_ADDRESS 0x0
#define OS_BUS_XDCI_SECTION_SIZE  0x100000
#endif /* FEATURE_HS_USB_SNPS_CORE */

enum
{
  HSU_OS_BUS_LPM_ENTRY_TIMEOUT = 1000,
  HSU_OS_BUS_LPM_ENTRY_TIMEOUT_UICC = 500,
  HSU_OS_BUS_RESUME_PERIPHERALS_TIMEOUT = 600
};

/* Maximum number of trials for setting the PHCD bit. */
const uint32  
  MAX_PHCD_BIT_WRITE_TRIALS = 500;

/* Maximum number of read trial of the PHCD bit, waiting for it to become 
** zero.
*/
const uint32  
  MAX_PHCD_BIT_READ_TRIALS = 500;

/* Map between core index used in hsu_os_abstract_bus, to HAL or 
core_config core index */
#define OS_BUS_TO_HAL_CORE_ID(os_bus_core_id) \
  (hsu_os_abstract_bus.hal_core_index_map_ptr[(os_bus_core_id)])

/* Map between core index used in HAL or core_config, to hsu_os_abstract_bus
core index */
#define OS_BUS_FROM_HAL_CORE_ID(hal_core_id) \
  (hsu_os_abstract_bus.hal_core_index_inv_map_ptr[(hal_core_id)])

/*==============================================================================
  Externalized Variables
==============================================================================*/

/*==============================================================================
  File Static Variables
==============================================================================*/
  /* The following memory is used to save the relevant USB registers
  ** in case of an ASSERT
  */
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
static uint32   otg_controller_mem_dump[OTG_REG_MEM_SIZE];
  #ifdef FEATURE_HS_USB_DUMP_USB_PHY_REG
  static uint8    ulpi_reg_mem_dump[ULPI_REG_MEM_SIZE];
  #endif
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */

#ifdef FEATURE_HS_USB_SNPS_CORE
static void hsu_os_bus_usb3_enter_lpm(uint32 core_index, 
                                      boolean called_by_lpm_timer,
                                      pwr_evnt_t power_event);

static void hsu_os_bus_usb3_exit_lpm(uint32 core_index, pwr_evnt_t power_event);
#endif /* FEATURE_HS_USB_SNPS_CORE */



static unsigned long hsu_os_intlock_flag;

static boolean device_resumed[NUMBER_OF_CORES];

/* Forward declarations of interrupt wrapper functions
** referenced  by the USB core table. */

#ifdef FEATURE_HS_USB_SNPS_CORE
void hsu_os_usb3_isr_wrapper(void);
void hsu_os_usb3_pwr_evt_irq_wrapper(void);
void hsu_os_usb3_hs_phy_isr_wrapper(void);
void hsu_os_usb2_phy_dpse_isr_wrapper(void);
void hsu_os_usb3_lfps_rxterm_isr_wrapper(void);
void hsu_os_usb3_lfps_rxterm_isr_pdc_wrapper(void);
#endif

#ifndef FEATURE_HS_USB_NO_CI_CORE
/* HSUSB Core HW Init */
static void otg_core_hw_init(void);
#endif // FEATURE_HS_USB_NO_CI_CORE

#ifdef FEATURE_HS_USB_SNPS_CORE
static void snps_core_hw_init(void);
#endif /* FEATURE_HS_USB_SNPS_CORE */

static void hsu_os_bus_init_core_index_maps(void);

static void hsu_os_hs_isr_wrapper_async_intr(void);


/*lint -e{785} 
** The resource lists are initialized with fewer elements
** than the full capacity of the array.
*/

/* USB core table.
** Contains the details of the USB
** cores installed in the system.
*/
static usb_core_info_type 
hsu_core_table[] = 
{
#ifdef FEATURE_HS_USB_SNPS_CORE
  /* SNPS core info. */
    {
      "SNPS USB Core",
      OS_BUS_USB_USB3_CORE,

      /* Number of controllers. */
      1,

      {
        /* SNPS USB core's device controller info. */
        {
          /* Controller type. Defined in 'controllers.h' */
          controller_type_snps_usb3_dcd,

          /* Controller handle. */
          NULL,

          /* Device controller's resource list. */
          {
            /* Device interrupt resource info. */
            {
              SYS_RES_IRQ,
              0,
              /* Device  controller interrupt resource.    */
              {
                {
                  USB3_CORE_INDEX,
                  NULL,
                  /*, NULL, NULL, NULL*/
                }
              }
            },

              /* Device memory resource info. */
            {
              SYS_RES_MEMORY,
              0,
                /* High-Speed USB OTG controller memory space. */
              {
                {
                  USB3_CORE_INDEX,
                  (juint8_t*)OS_BUS_XDCI_START_ADDRESS,
                  OS_BUS_XDCI_SECTION_SIZE
                }
              }
            }
          } /* End of resource list. */
        }  /* End of device controller info. */
      },  /* End of core controllers list. */

        /* SNPS core interrupt number. */
        (uint32)TRAMP_USB3_EE1_IRQ, /* In Virtio, re-use HS-USB tramp */

        /* SNPS core interrupt wrapper function. */
        hsu_os_usb3_isr_wrapper

        /* Core's LPM information */
        ,{    
          TRUE,   /* LPM feature is active */    
          FALSE,  /* Not in low-power mode.                             */
          FALSE,  /* Cable connected during LPM (a don't care init).    */
          FALSE,  /* LPM entry timer is not armed.                      */
          HSU_OS_BUS_LPM_ENTRY_TIMEOUT, /* LPM timer length             */
          0,    /* An initializer for low-power mode entry timer.     */
          {0},    /* An initializer for the remote wakeup timer.        */
          0,    /* An initializer for cable status check timer.       */
          FALSE,  /* exit due to remote wakeup.                         */
          FALSE,  /* is_remote_wakeup_pending                           */
          0,      /* remote_wakeup_suspend_time_stamp_ms                */
          hsu_os_bus_usb3_enter_lpm, /* Enter LPM handler function             */  
          hsu_os_bus_usb3_exit_lpm,  /* Exit LPM handler function              */
          hsu_port_is_attached,  /* Port connection status check function. */
          TRUE /* enable cable status check */
        }  /* End of core LPM information. */

        ,snps_core_hw_init /* SNPS Core Hardware Init */


    },  /* End of SNPS core info. */
#endif /* FEATURE_HS_USB_SNPS_CORE */
};  /* hsu_core_table */

/* Map between core index used in hsu_os_abstract_bus, to HAL or 
   core_config core index */
static uint8 hsu_hal_core_index_map[NUMBER_OF_CORES];

/* Map between core index used in HAL or core_config, to hsu_os_abstract_bus
   core index */
static uint8 hsu_hal_core_index_inv_map[HSU_MAX_NUM_OF_CORES];

static hsu_os_bus_statistics_type
  hsu_os_bus_statistics = 
  {
    0,  /* Spoofed reconnections count - Init. */
    0,  /* PHY ascess fail count - Init */
    0   /* PHY resets count due to access failure - Init*/
  };  /* hsu_os_bus_statistics */

static os_bus_file_static_vars_type
  hsu_os_abstract_bus =
  {
    hsu_core_table,
    hsu_hal_core_index_map,
    hsu_hal_core_index_inv_map,
    &hsu_os_bus_statistics,

    //(pm_vote_vreg_app_type)0, /* PMIC VREG vote handle, initial value. */ // 9x25 BU

    TRUE,   /* Enable PHY LPM feature.                           */

    FALSE,

    #ifndef T_UNDP
    TRUE,   /* Enable usbh_pclk control.                         */
    #else
    FALSE,  /* Disable usbh_pclk control on the U2 target,
            ** since it works only when the ULPI_STP_CTRL bit
            ** is used.
            */
    #endif  /* !T_UNDP */

    TRUE,   /* Enable GPIO control.                              */
    TRUE,   /* Enable dynamic clock rate switching.              */
    {0},    /* An initializer for the lpm_crit_sect struct.      */

    #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
    FALSE,
    #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

    #ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
    otg_controller_mem_dump,  /* Memory area for saving the OTG 
                                  Controller registers */
    #ifdef FEATURE_HS_USB_DUMP_USB_PHY_REG
    ulpi_reg_mem_dump,             /* Memory area for saving the ULPI registers */
    #endif
    #endif  /* !FEATURE_HS_USB_MEM_OPTIMIZATION */

    0, /* remote_wakeup_delay */
}; /* hsu_os_abstract_bus */

static os_bus_file_static_vars_type* const
  file_static_vars_ptr = &hsu_os_abstract_bus;

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================

  FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_OS_BUS_INTERRUPT_INIT

DESCRIPTION
  Initializes a USB core's interrupt. Called when the core's interrupt is
  registered its first interrupt service routine (using the 
  hsu_os_interrupt_setup() call).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_interrupt_init(uint32 core_index)
{
  usb_core_info_type* const core_info_ptr =
    &(file_static_vars_ptr->core_table_ptr[core_index]);

  void    (*core_interrupt_handler_ptr) (void);

  DALResult eRes = 0;

  uint32  core_interrupt_number;

  DalDeviceHandle *hsu_h_int_cntlr = NULL;

  HSU_ULOG_1(HIGH_MSG, BUS_LOG, "core_index %u", core_index);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  if (core_info_ptr->num_of_registered_isrs == 0)
  {
    /* If there are no registered ISRs for this core,
    ** do nothing. This might occur when the stack is inactive
    ** and then LPM is activated.
    */
    HSU_MSG_HIGH("hsu_os_bus_interrupt_init: There are no registered ISRs. "
      "Do nothing.");

    return HSU_JSUCCESS;
  }

  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hsu_h_int_cntlr);
  HSU_ASSERT(hsu_h_int_cntlr != NULL && eRes == DAL_SUCCESS);

  core_interrupt_handler_ptr = core_info_ptr->interrupt_handler;

  core_interrupt_number      = core_info_ptr->interrupt_number;

  /* Target platform implementation. */

  HSU_MSG_HIGH_1("hsu_os_bus_interrupt_init: Setting interrupt on Tramp "
               "interrupt %d.", 
               core_interrupt_number);

  eRes = DalInterruptController_RegisterISR(
      hsu_h_int_cntlr,
      core_interrupt_number,
      (DALISR) core_interrupt_handler_ptr,
      NULL,
      DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);

  HSU_ASSERT(eRes == DAL_SUCCESS);

  if(core_info_ptr->core_type == OS_BUS_USB_USB3_CORE)
  {
    /*eRes = DalInterruptController_RegisterISR(
      hsu_h_int_cntlr,
      TRAMP_USB_HS_WAKEUP_IRQ,
      (DALISR) hsu_os_hs_isr_wrapper_async_intr,
      NULL,
      DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);*/

    eRes = DalInterruptController_RegisterISR(
      hsu_h_int_cntlr,
      TRAMP_USB_QUSB2_PHY_IRQ,
      (DALISR) hsu_os_hs_isr_wrapper_async_intr,
      NULL,
      DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);

    eRes = DalInterruptController_RegisterISR(
      hsu_h_int_cntlr,
      TRAMP_USB3_POWER_EVENT_IRQ,
      (DALISR) hsu_os_usb3_pwr_evt_irq_wrapper,
      NULL,
      DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);

    HSU_ASSERT(eRes == DAL_SUCCESS);

    /*eRes = DalInterruptController_RegisterISR(
      hsu_h_int_cntlr,
      TRAMP_USB_HS_PHY_IRQ,
      (DALISR) hsu_os_usb3_hs_phy_isr_wrapper,
      NULL,
      DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);*/
  }
  else
  {
    HSU_ULOG_1(HIGH_MSG, DBG_E_LOG, "write core idx %u", core_index);
  }

  HSU_ASSERT(eRes == DAL_SUCCESS);

  HSU_MSG_HIGH("hsu_os_bus_interrupt_init: Exit.");

  return HSU_JSUCCESS;
}  /* hsu_os_bus_interrupt_init */


/*===========================================================================
FUNCTION HSU_OS_BUS_INTERRUPT_UNINIT

DESCRIPTION
  Un-initializes High-Speed USB interrupt. Called by the bus module
  un-initialization function (hsu_os_bus_uninit).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_interrupt_uninit(uint32 core_index)
{
  usb_core_info_type* const core_info_ptr =
    &(file_static_vars_ptr->core_table_ptr[core_index]);

  uint32  core_interrupt_number;
  DalDeviceHandle *hsu_h_int_cntlr = NULL;
  DALResult eRes = 0;

  HSU_ULOG_1(HIGH_MSG, BUS_LOG, "core_index %u", core_index);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_interrupt_number = core_info_ptr->interrupt_number;

  /* Target platform implementation. */
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hsu_h_int_cntlr);
  HSU_ASSERT(hsu_h_int_cntlr != NULL && eRes == DAL_SUCCESS);

  eRes = DalInterruptController_Unregister(
    hsu_h_int_cntlr,
    (uint32) core_interrupt_number);

  HSU_ASSERT(eRes == DAL_SUCCESS);

  if(core_info_ptr->core_type == OS_BUS_USB_HIGH_SPEED_CORE)
  {
    /*eRes = DalInterruptController_Unregister(
      hsu_h_int_cntlr,
      (uint32) TRAMP_USB_HS_WAKEUP_IRQ);*/

    eRes = DalInterruptController_Unregister(
      hsu_h_int_cntlr,
      (uint32) TRAMP_USB_QUSB2_PHY_IRQ);

    eRes = DalInterruptController_Unregister(
      hsu_h_int_cntlr,
      (uint32) TRAMP_USB3_POWER_EVENT_IRQ);

    /*eRes = DalInterruptController_Unregister(
      hsu_h_int_cntlr,
      (uint32) TRAMP_USB_HS_PHY_IRQ);*/
  }
  else
  {
    HSU_ULOG_1(HIGH_MSG, DBG_E_LOG, "write core idx %u", core_index);
  }
  HSU_ASSERT(eRes == DAL_SUCCESS);

  HSU_MSG_HIGH("hsu_interrupt_uninit: Exit.");

  return HSU_JSUCCESS;

}  /* hsu_os_bus_interrupt_uninit */

#ifndef CI_MERGE
#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE)
/*===========================================================================
FUNCTION HSU_OS_BUS_READ_WRITE_PHY

DESCRIPTION
  Perform a read or write access to a PHY register using the USB core's 
  ULPI_VIEWPORT register.

DEPENDENCIES
  Assumes that the core operates in ULPI mode.
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY access operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_read_write_phy_impl(boolean  is_write_operation,
                                         uint8    register_address,
                                         uint8*   register_value,
                                         boolean  reset_phy_on_fail,
                                         uint32   core_idx)
{
  boolean  phy_access_is_successful;
  void* core_index_ptr;
  uint32 core_index = OS_BUS_FROM_HAL_CORE_ID(core_idx); //use logical core index (not HAL core index)

  core_index_ptr =
    (void *)&(file_static_vars_ptr->core_table_ptr[core_index].controller_info_ptr->
    resource_list[core_index].resource_instance.mem_resource_instance.core_index);

  HSU_USE_PARAM(core_index_ptr);

  /* Check whether the PHY is in low power mode */
  if (HAL_hsusb_GetBits32(
    OS_BUS_TO_HAL_CORE_ID(core_index),
    HAL_HSUSB_PORTSC_ADDR(0),HAL_HSUSB_PORTSC_PHCD_BMSK) != 0)
  {
    HSU_ERR_FATAL("hsu_os_bus_read_write_phy_impl: Access to core registers during "
                  "low-power mode is not possible. register: %X",
                  register_address, 0, 0);
    return FALSE;
  }

  if (is_write_operation)
  {
    phy_access_is_successful = (HAL_hsusb_WriteUlpiPhy(
      OS_BUS_TO_HAL_CORE_ID(core_index), 0, 
      register_address, *register_value) == HAL_HSUSB_Success);
  }
  else
  {
    phy_access_is_successful = (HAL_hsusb_ReadUlpiPhy(
      OS_BUS_TO_HAL_CORE_ID(core_index), 0, 
      register_address, register_value) == HAL_HSUSB_Success);
  }

  if (!phy_access_is_successful) 
  {
    file_static_vars_ptr->hsu_os_bus_statistics_ptr->phy_access_fail_count++;
    if ((reset_phy_on_fail) && (file_static_vars_ptr->reset_phy_enabled))
    {
      HSU_MSG_ERROR("hsu_os_bus_read_write_phy_impl: Enqueuing hsu_conf_sel_stack_utils_device_restart_core"
        " to AL task");
      file_static_vars_ptr->hsu_os_bus_statistics_ptr->stuck_phy_resets_count++;
      hsu_al_task_enqueue_cmd(hsu_conf_sel_stack_utils_device_restart_core, (void *)core_index);
    }
  }
  return phy_access_is_successful;  
}  /* hsu_os_bus_read_write_phy_impl */

/*===========================================================================
FUNCTION HSU_OS_BUS_READ_FROM_PHY_IMPL

DESCRIPTION
  Read from a PHY register using the USB core's ULPI_VIEWPORT register.
  (The implementation)
  If the PHY assess fails and reset_phy=TRUE - resets the core

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY read operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_read_from_phy_impl
(
 uint8 phy_register, 
 uint8* read_value,
 boolean reset_phy,
 uint32 core_index
)
{
  boolean phy_access_is_successful;


  if (read_value == NULL)
  {
    HSU_MSG_ERROR("hsu_os_bus_read_from_phy_impl: Illegal NULL parameter.");

    HSU_ASSERT(FALSE);

    return FALSE;
  }
  
  HSU_MSG_LOW_1("hsu_os_bus_read_from_phy_impl: Reading from register %X", phy_register);

  phy_access_is_successful = 
    hsu_os_bus_read_write_phy_impl(FALSE, phy_register, read_value,reset_phy,core_index);

  if (!phy_access_is_successful)
  {
    HSU_MSG_ERROR("hsu_os_bus_read_from_phy_impl: PHY read operation failed.");
  }

  return phy_access_is_successful;

}  /* hsu_os_bus_read_from_phy_impl */
/*===========================================================================
FUNCTION HSU_OS_BUS_READ_FROM_PHY

DESCRIPTION
  Read from a PHY register using the USB core's ULPI_VIEWPORT register.
  If the PHY assess fails - resets the core

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY read operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_read_from_phy
( 
 uint8 phy_register, 
 uint8* read_value,
 uint32 core_index
 )
{
  return hsu_os_bus_read_from_phy_impl(phy_register,read_value,TRUE,core_index);
}

/*===========================================================================
FUNCTION HSU_OS_BUS_READ_FROM_PHY_NO_RESET

DESCRIPTION
  Read from a PHY register using the USB core's ULPI_VIEWPORT register.
  If the PHY assess fails - DOESNT reset the core

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY read operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_read_from_phy_no_reset
( 
 uint8 phy_register, 
 uint8* read_value,
 uint32 core_index
 )
{
  return hsu_os_bus_read_from_phy_impl(phy_register,read_value,FALSE,core_index);
}

/*===========================================================================
FUNCTION HSU_OS_BUS_WRITE_TO_PHY_IMPL

DESCRIPTION
  Write to a PHY register using the USB core's ULPI_VIEWPORT register.
  (The implementation)
  If the PHY assess fails and reset_phy=TRUE - resets the core

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY write operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_write_to_phy_impl
(
 uint8 phy_register, 
 uint8 written_value,
 boolean reset_phy,
 uint32 core_index
)
{
  boolean phy_access_is_successful;

  HSU_MSG_LOW_2("hsu_os_bus_write_to_phy: Writing %X to register %X",
               written_value, phy_register);

  phy_access_is_successful = 
    hsu_os_bus_read_write_phy_impl(TRUE, phy_register, &written_value,reset_phy,core_index);

  if (!phy_access_is_successful)
  {
    HSU_MSG_ERROR("hsu_os_bus_write_to_phy_impl: PHY write operation failed.");
  }

  return phy_access_is_successful;

}  /* hsu_os_bus_write_to_phy_impl */
/*===========================================================================
FUNCTION HSU_OS_BUS_WRITE_TO_PHY

DESCRIPTION
  Write to a PHY register using the USB core's ULPI_VIEWPORT register.
  If the PHY assess fails - resets the core

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY write operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_write_to_phy
(
 uint8 phy_register, 
 uint8 written_value,
 uint32 core_index
 )
{
  return  hsu_os_bus_write_to_phy_impl(phy_register,written_value,TRUE,core_index);
} /*hsu_os_bus_write_to_phy*/


#endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE*/
#endif  /*CI_MERGE*/

/*===========================================================================
FUNCTION HSU_OS_BUS_LFPS_INTERRUPT_UNINIT
===========================================================================*/
jresult_t hsu_os_bus_lfps_interrupt_uninit(uint32 core_index)
{
  DalDeviceHandle *hsu_h_int_cntlr = NULL;
  DALResult eRes = 0;

  HSU_ULOG_1(NO_MSG, BUS_LOG, "core_index %u", core_index);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  /* Target platform implementation. */
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hsu_h_int_cntlr);
  HSU_ASSERT(hsu_h_int_cntlr != NULL && eRes == DAL_SUCCESS);

  eRes = DalInterruptController_Unregister(
    hsu_h_int_cntlr,
    (uint32) TRAMP_USB_QMP_PCIE_USB3_LFPS_RXTERM_CX_IRQ);
  HSU_ASSERT(eRes == DAL_SUCCESS);

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION HSU_OS_BUS_PWR_EVNT_INTERRUPT_UNINIT
===========================================================================*/
jresult_t hsu_os_bus_pwr_evnt_interrupt_uninit(uint32 core_index)
{
  DalDeviceHandle *hsu_h_int_cntlr = NULL;
  DALResult eRes = 0;

  HSU_MSG_HIGH("hsu_os_bus_pwr_evnt_interrupt_uninit: Entry.");
  HSU_ULOG_1(NO_MSG, BUS_LOG, "core_index %u", core_index);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  /* Target platform implementation. */
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hsu_h_int_cntlr);
  HSU_ASSERT(hsu_h_int_cntlr != NULL && eRes == DAL_SUCCESS);

  eRes = DalInterruptController_Unregister(
    hsu_h_int_cntlr,
    (uint32) TRAMP_USB3_POWER_EVENT_IRQ);
  HSU_ASSERT(eRes == DAL_SUCCESS);

  return HSU_JSUCCESS;
}

jresult_t hsu_os_bus_usb2_phy_interrupt_uninit(uint32 core_index)
{
  DalDeviceHandle *hsu_h_int_cntlr = NULL;
  DALResult eRes = 0;

  HSU_MSG_HIGH("hsu_os_bus_usb2_phy_interrupt_uninit: Entry.");
  HSU_ULOG_1(NO_MSG, BUS_LOG, "core_index %u", core_index);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  /* Target platform implementation. */
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hsu_h_int_cntlr);
  HSU_ASSERT(hsu_h_int_cntlr != NULL && eRes == DAL_SUCCESS);

  eRes = DalInterruptController_Unregister(
    hsu_h_int_cntlr,
    (uint32) TRAMP_USB_QUSB2_PHY_IRQ);
  HSU_ASSERT(eRes == DAL_SUCCESS);

  return HSU_JSUCCESS;
}


/*===========================================================================
FUNCTION HSU_OS_BUS_ENTER_LPM

DESCRIPTION
  Enter High-Speed USB PHY into low-power mode state.
  This function is invoked when the High-Speed USB bus is suspended, in 
  order to save battery power.
  This function stops the High-Speed USB core PHY clock and enables
  asynchronous interrupts from the PHY.
  After the PHY is entered into a low power-state, this function asserts
  the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef FEATURE_HS_USB_NO_CI_CORE
void hsu_os_bus_enter_lpm(
  uint32 core_index, 
  boolean called_by_lpm_timer,
  pwr_evnt_t power_event
)
{
  hsu_lpm_ctx_type *lpm_ctx = hsu_lpm_get_ctx();

  HSU_USE_PARAM(power_event);

  /* initialize lpm context */
  lpm_ctx->core_idx = core_index;
  lpm_ctx->is_called_by_lpm_timer = called_by_lpm_timer;
  lpm_ctx->is_lpm_feature_active = file_static_vars_ptr->lpm_feature_is_active;
  lpm_ctx->crit_sec = &file_static_vars_ptr->lpm_crit_sect;
  lpm_ctx->lpm_info =
    &file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info;
  
  hsu_lpm_enter(lpm_ctx);
}
#endif // FEATURE_HS_USB_NO_CI_CORE


/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_LPM

DESCRIPTION
  Exit High-Speed USB PHY from the low-power mode state.
  This function is invoked when the High-Speed USB bus is resumed from a
  suspend state, in order to return the High-Speed USB PHY back to an 
  operational state.
  This function negates the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef FEATURE_HS_USB_NO_CI_CORE
void hsu_os_bus_exit_lpm(uint32 core_index, pwr_evnt_t power_event)
{
  hsu_lpm_ctx_type *lpm_ctx = hsu_lpm_get_ctx();
  
  HSU_USE_PARAM(power_event);
  
  /* initialize lpm context */
  lpm_ctx->core_idx = core_index;
  lpm_ctx->is_lpm_feature_active = file_static_vars_ptr->lpm_feature_is_active;
  lpm_ctx->crit_sec = &file_static_vars_ptr->lpm_crit_sect;
  lpm_ctx->lpm_info =
    &file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info;
  
  hsu_lpm_exit(lpm_ctx);
}
#endif // FEATURE_HS_USB_NO_CI_CORE

#if defined(FEATURE_HS_USB_SNPS_CORE)

/*===========================================================================
FUNCTION HSU_OS_BUS_USB3_ENTER_LPM

DESCRIPTION
Enter USB3 core into low-power mode state.
This function is invoked when the USB3 bus is suspended, in 
order to save battery power.
After the USB3 core is entered into a low power-state, this function asserts
the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_bus_usb3_enter_lpm
(
 uint32 core_index, 
 boolean called_by_lpm_timer,
 pwr_evnt_t pwr_event
)
{

  hsu_lpm_snps_ctx_type *lpm_ctx = hsu_lpm_get_snps_ctx();

  HSU_USE_PARAM(called_by_lpm_timer);

  lpm_ctx->crit_sec = &file_static_vars_ptr->lpm_crit_sect;

  rex_enter_crit_sect(lpm_ctx->crit_sec);

  /* initialize lpm context */
  lpm_ctx->core_index = core_index;
  lpm_ctx->is_lpm_feature_active = file_static_vars_ptr->lpm_feature_is_active;
  lpm_ctx->power_event = pwr_event;
  
  lpm_ctx->lpm_info =
    &file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info;
   
  hsu_lpm_snps_enter(lpm_ctx); 

  rex_leave_crit_sect(lpm_ctx->crit_sec);
 
}  /* hsu_os_bus_usb3_enter_lpm */

/*===========================================================================
FUNCTION HSU_OS_BUS_USB3_EXIT_LPM

DESCRIPTION
Exit USB3 core from the low-power mode state.
This function is invoked when the USB3 bus is resumed from a
suspend state, in order to return the USB3 core back to an 
operational state.
This function negates the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_bus_usb3_exit_lpm(uint32 core_index, pwr_evnt_t power_event)
{
  hsu_lpm_snps_ctx_type *lpm_ctx = hsu_lpm_get_snps_ctx();
 
  lpm_ctx->crit_sec = &file_static_vars_ptr->lpm_crit_sect;

  rex_enter_crit_sect(lpm_ctx->crit_sec);
  
  /* initialize lpm context */
  lpm_ctx->core_index = core_index;
  lpm_ctx->is_lpm_feature_active = file_static_vars_ptr->lpm_feature_is_active;
  lpm_ctx->power_event = power_event;

  lpm_ctx->lpm_info =
    &file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info;
  
  hsu_lpm_snps_exit(lpm_ctx);

  rex_leave_crit_sect(lpm_ctx->crit_sec);

}  /* hsu_os_bus_usb3_exit_lpm */

#endif /* FEATURE_HS_USB_SNPS_CORE */


/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME_PERIPHERALS_TIMER_CB

DESCRIPTION

  This function is called when the resume_peripherals timer expires. 
  This timer is armed when the core exits from low-power mode
  due to an asynchronous interrupt. When this happens, if we are in host mode
  resume all of the connected peripherals sequence is scheduled. 
  This operation is delayed (via timer) in order to give the hs-usb0
  task enough time to update the cable connection status. 

  This function resumes all of the connected peripherals after remote wakeup
  was received (host mode only).

  This is a SW workaround for the following scenario:
  We're operating in host mode and suspend the bus. As a result of the 
  suspend we enter LPM and vote for sleep. TCXO shutdown occurs. One of 
  the connected peripherals issues remote wakeup. According to the standard 
  the peripheral should hold the remote wakeup signal 1-15 msec (see section 
  7.1.7.7 in USB2.0). This time interval isn’t enough for us to exit TCXO 
  shutdown and thus we "miss" the  remote wakeup signal.

  This function is called in the AL task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If in host mode - All the connected peripherals are resumed.
===========================================================================*/
static void hsu_os_bus_resume_peripherals_timer_cb(unsigned long param)
{
  HSU_USE_PARAM(param);
  /* If we're operating in host_mode this function could be called
  ** only due to remote_wakeup being issued by one of the connected 
  ** peripherals. In such case, resume all the connected peripherals
  */
  #ifdef FEATURE_HS_USB_OTG 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  if (hsu_otg_app_is_mini_a_connected())
  {
    /* When calling this function we use hsu_al_task_enqueue_cmd() so it's
    ** always called from handle_al_task_commands() where we lock the
    ** giant mutex so no need to do it here.
    */
    HSU_MSG_HIGH("hsu_os_bus_resume_peripherals(): locked mutex - "
                 "resuming peripherals.");

    if (generic_controller_resume(
          file_static_vars_ptr->core_table_ptr[HS_USB_CORE_INDEX].
            controllers_list[0].controller_handle) != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("hsu_os_bus_resume_peripherals(): Host mode - "
                    " Failed to resume all connected peripherals");
    }
    else
    {
      HSU_MSG_HIGH("hsu_os_bus_resume_peripherals(): Host mode - "
                   "Resumed all of the connected peripherals.");
    }
  }
  #endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif /*FEATURE_HS_USB_OTG*/
}/*hsu_os_bus_resume_peripherals*/

#ifndef FEATURE_HS_USB_NO_CI_CORE
/*===========================================================================
FUNCTION HSU_OS_BUS_SPOOF_RECONNECT

DESCRIPTION
This function causes the device to re-connect to the host.
Specifically, this function issue a spoof disconnect, waits some 
time to ensure that the disconnection is detected by the host, and then issues 
a spoof connect.

Note - This function assumes the Giant mutex is locked upon calling it, and 
therefore should be called from the AL task context only.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_os_bus_spoof_reconnect(uint32 core_index)
{
  static uint32 hsu_os_bus_spoof_reconnect_delay_ms = 500;
    /* The delay between the spoofed disconnect to the spoofed connect.
    ** Should be long enough to allow the host to distinguish the spoofed 
    ** disconnection.
    */


  if (!hsu_phy_utils_spoof_disconnect_core(OS_BUS_TO_HAL_CORE_ID(core_index)))
  {
    HSU_MSG_ERROR("hsu_os_bus_spoof_reconnect: "
      "Failed do spoof USB cable disconnection");

    return FALSE;
  }

  hsu_os_delay_ms(hsu_os_bus_spoof_reconnect_delay_ms);

  if (!hsu_phy_utils_spoof_connect_core(OS_BUS_TO_HAL_CORE_ID(core_index)))
  {
    HSU_MSG_ERROR("hsu_os_bus_spoof_reconnect_ex: "
      "Failed do spoof USB cable connection");

    return FALSE;
  }

  return TRUE;
}  /* hsu_os_bus_spoof_reconnect */
#endif // FEATURE_HS_USB_NO_CI_CORE

/*===========================================================================
FUNCTION HSU_OS_BUS_SPOOF_RECONNECT_AL_TASK_COMMAND

DESCRIPTION
  This function is a callback function executed by the AL task.
  This function checks whether a reset event has occurred, and if so,
  emulates a USB cable re-connection event. It is used when there is a 
  USB bus reset during USB suspend state and the HS-USB module is in LPM.

  This function is needed because sometimes it takes a long time for the phone 
  to wake up from TCXO shutdown state when the USB bus reset event occurs. As a 
  result, the USB chirp timing is violated and the USB bus runs at full-speed 
  mode. Therefore, this function is used to force a new attachment of the phone
  to the host, this time at High-Speed mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void 
hsu_os_bus_spoof_reconnect_al_task_command(void* command_param)
{
  static uint32 hsu_os_bus_usb_reset_bit_update_delay_ms = 2;

  uint32 core_index = (uint32)command_param;


  /* Let the USBCMD register USB reset bit stabilize. */
  hsu_os_delay_ms(hsu_os_bus_usb_reset_bit_update_delay_ms);

  //TODO HSU Need to implement spoof reconnect for SNPS core
#ifndef FEATURE_HS_USB_NO_CI_CORE
  /* Check whether a USB bus reset event has occurred. */
  if (HAL_hsusb_GetBits32(OS_BUS_TO_HAL_CORE_ID(core_index), 
                          HAL_HSUSB_USBSTS_ADDR, 
                          HAL_HSUSB_USBSTS_URI_BMSK) != 0)
  {
    /* Emulate a reconnection of the USB cable. */
    if (!hsu_os_bus_spoof_reconnect(core_index))
    {
      HSU_MSG_ERROR("hsu_os_bus_spoof_reconnect: "
                    "Failed do spoof USB cable connection");
    }
    else
    {
      file_static_vars_ptr->hsu_os_bus_statistics_ptr->
        spoofed_reconnects_on_reset_during_suspend_count++;

      HSU_MSG_HIGH("hsu_os_bus_spoof_reconnect_al_task_command: "
                  "Successfully spoofed USB reconnection event.");
    }
  }
#endif // FEATURE_HS_USB_NO_CI_CORE
  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);

}  /* hsu_os_bus_spoof_reconnect_al_task_command */


/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_LPM_BY_INTR

DESCRIPTION
  This function is queued to the DPC task when an HS-USB interrupt occurs 
  during low-power mode, and is responsible for low-power mode exiting by an
  HS-USB interrupt. 

  The low-power power mode exit procedure involves RPC calls, mutex lock\unlock,
  and it is too heavy to be performed from an interrupt context. Therefore, the
  interrupt context queues this function to the DPC task, and un-registers 
  itself from the Tramp. When this functions executes, it performs the low-power 
  mode exit procedure, and registers the HS-USB interrupt bask in the Tramp, 
  in order to let the rest of the HS-USB interrupt events to be handled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
//USB_THREADX
#ifdef FEATURE_L4
void hsu_os_bus_exit_lpm_by_intr(unsigned long core_index)
#else
boolean hsu_os_bus_exit_lpm_by_intr(unsigned long core_index,
                                           unsigned long unused_param2,
                                           unsigned long unused_param3)
#endif
{
  hsu_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  #ifndef FEATURE_L4
  HSU_USE_PARAM(unused_param2);
  HSU_USE_PARAM(unused_param3);
  #endif  /* FEATURE_L4 */

  if (core_lpm_info->exit_lpm_handler)
  {
    core_lpm_info->exit_lpm_handler(core_index, PWR_EVNT_SW);
  }

  // Log XO shutdown and VDD MIN count
  //hsu_platform_log_xosd_and_vddmin_info();

  /* If the spoof reconnect on reset during suspend feature
  ** is enabled, check whether the USB reset bit is set in the 
  ** USB status register. If this bit is set, a spoofed reconnection
  ** event is issued.
  **
  ** This feature fails the USBCV tests. Therefore, it is not 
  ** activated when is configured for running under USBCV. That
  ** means, when the Remote Wakeup Delay NV item (used only for NV tests)
  ** is non-zero.
  */

  if (core_index == HS_USB_CORE_INDEX)
  {
    if ( hsu_conf_sel_reconnect_on_reset_during_suspend() &&
        (hsu_conf_sel_get_rm_wakeup_delay() == 0) )
    {
      if (core_lpm_info->cable_is_connected_during_lpm)
      {
        hsu_al_task_enqueue_cmd(hsu_os_bus_spoof_reconnect_al_task_command, 
                                (void*)core_index);

        #ifdef FEATURE_L4
        return;
        #else
        return TRUE;
        #endif /* FEATURE_L4 */
      }
    }
  }

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm_by_intr: scheduling cable status check");

  if (core_index == HS_USB_CORE_INDEX)
  {
#ifndef FEATURE_HS_USB_NO_CI_CORE
#ifdef FEATURE_HS_USB_OTG
    /* In host mode, schedule timer to resume all peripherals */
    if (hsu_otg_app_get_otg_mode() == OTG_DEVICE_MODE_HOST)
    {
      hsu_timer_set(
        &(file_static_vars_ptr->core_table_ptr[core_index].resume_peripherals_timer),
        HSU_OS_BUS_RESUME_PERIPHERALS_TIMEOUT,
        T_MSEC);      
    }
#endif /* FEATURE_HS_USB_OTG */
#endif // FEATURE_HS_USB_NO_CI_CORE

    /* If the High-Speed USB was exited from low-power mode due to 
    ** an interrupt, schedule a USB cable status check in order to
    ** decide whether to return to low-power mode.
    */
    hsu_timer_set(
      &(core_lpm_info->cable_status_check_timer),
      core_lpm_info->lpm_entry_timer_value_ms,
      T_MSEC);

    core_lpm_info->enable_cable_status_check = TRUE;
  }
  #ifndef FEATURE_L4
  return TRUE;
  #endif  /* !FEATURE_L4 */

}  /* hsu_os_bus_exit_lpm_by_intr */

/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_DISABLE

DESCRIPTION
  Disable the LPM feature. Exit from low-power mode if needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_lpm_disable(uint32 core_index)
{
  hsu_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
  if (core_lpm_info->exit_lpm_handler)
  {
    core_lpm_info->exit_lpm_handler(core_index, PWR_EVNT_SW);
  }
  core_lpm_info->lpm_is_active = FALSE;

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
}  /* hsu_os_bus_lpm_disable */


/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_ENABLE

DESCRIPTION
  Enable the low-power mode feature.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_lpm_enable(uint32 core_index)
{
  hsu_lpm_info_type* core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  
  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
  core_lpm_info->lpm_is_active = TRUE;
  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (core_index == HS_USB_CORE_INDEX)
  {
    /* Schedule a USB cable status check. */
   
    hsu_timer_set(
      &(core_lpm_info->cable_status_check_timer),
      core_lpm_info->lpm_entry_timer_value_ms,
      T_MSEC);  

    core_lpm_info->enable_cable_status_check = TRUE;
  }
}  /* hsu_os_bus_lpm_enable */

/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_ENTRY_TIMER_CB

DESCRIPTION
Called when the Low Power Mode entry timer expires. 
Enters the core into low power mode.
The function needs to be queued to AL task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_lpm_entry_timer_cb(unsigned long para)
{
  unsigned long core_index = para;
  hsu_lpm_info_type* core_lpm_info;

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  if (core_lpm_info->enter_lpm_handler)
  {
    core_lpm_info->enter_lpm_handler(core_index, TRUE, PWR_EVNT_SW);
  }
}


/*===========================================================================
FUNCTION HSU_OS_BUS_RM_WAKEUP_COMMAND

DESCRIPTION
  Called when the HSU AL task is ready to handle remote wakeup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_rm_wakeup_command(void* cmd_param)
{
  juint8_t controller_idx = (juint8_t)((uint32)cmd_param);

  HSU_ASSERT((uint32)cmd_param == (uint32)controller_idx);

  jsafe_enter();
  /* cmd_param (logical index) need to be convered to HAL index */
  (void)(jslave_wakeup_host(hsu_os_bus_hal_core_id(controller_idx)));
  jsafe_leave();

} /* hsu_os_bus_rm_wakeup_command */


/*===========================================================================
FUNCTION HSU_OS_BUS_RM_WAKEUP_TIMER_CB_DEFERRED

DESCRIPTION
  Called when the remote wakeup timer expires in AL TASK.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_rm_wakeup_timer_cb(unsigned long callback_param)
{
  uint32 core_index = (uint32)callback_param;
  hsu_lpm_info_type* const core_lpm_info = &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  boolean is_remote_wakeup_pending;
  is_remote_wakeup_pending = core_lpm_info->is_remote_wakeup_pending;

  if (is_remote_wakeup_pending)
  {
    core_lpm_info->is_remote_wakeup_pending = FALSE;
  }

  HSU_ULOG_1(NO_MSG, BUS_LOG, "is_remote_wakeup_pending %u", is_remote_wakeup_pending);

  /* In case NV delay value is set to 0, no need to explicitly 
  ** issue a remote wakeup signal. 
  */
  if ((hsu_conf_sel_get_rm_wakeup_delay() != 0)
     || (is_remote_wakeup_pending))
  {
    if(SPEED_SUPER == core_get_curr_speed(core_index))
    {
      core_function_request_remote_wakeup_all_iface(core_index);
    }
    else
    {
      /* jslave_wakeup_host might block, therefore, need to schedule it 
      ** in the AL task
      */
      hsu_os_bus_rm_wakeup_command((void*)callback_param);
    }
  }
}
/*===========================================================================
FUNCTION HSU_PORT_IS_ATTACHED

DESCRIPTION
  This function checks whether the High-Speed USB port is attached.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the high-speed USB port is attached. Otherwise, returns FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_port_is_attached(uint32 core_index)
{
  boolean return_value = FALSE;

  usb_core_info_type* core_ptr = 
    &(file_static_vars_ptr->core_table_ptr[core_index]);
  
  hsu_lpm_info_type* const core_lpm_info = &(core_ptr->usb_lpm_info);

  /* If we are in low-power mode, register access is denied. */
  if (core_lpm_info->is_in_low_power_mode)
  {
    /*
    ** Return the cable status recorded just before the LPM entry.
    */
    return core_lpm_info->cable_is_connected_during_lpm;
  }

  return_value = hsu_vregmgr_is_usb_cable_attached();
  return return_value;

}  /* hsu_port_is_attached */

/*===========================================================================
FUNCTION HSU_OS_BUS_CABLE_STATUS_CHECK_TIMER_CB_QED

DESCRIPTION
  Called when High-Speed USB cable status check timer expires. 
  Checks whether the High-Speed USB port is attached.
  If it isn't, enters the core into low power mode.
  The function needs to be invoked in AL TASK

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_cable_status_check_timer_cb
(
 unsigned long para
)
{
  uint32 core_index = (uint32) para;
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: Entry");

  if (core_lpm_info->enable_cable_status_check == FALSE)
  {
    HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: cable check disabled");
    return;
  }

  if (core_lpm_info->port_is_attached  &&
      !core_lpm_info->port_is_attached(core_index))
  {
    HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: entering LPM");
    if (core_lpm_info->enter_lpm_handler)
    {
      core_lpm_info->enter_lpm_handler(core_index, FALSE, PWR_EVNT_SW);
    }
  }

  HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: Exit");
} /* hsu_os_bus_cable_status_check_timer_cb */


/*===========================================================================
FUNCTION HSU_OS_ISR_WRAPPER_INT

DESCRIPTION
  A common code for a wrapper function that handles an USB interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_isr_wrapper_int
(
  uint32 core_index
)
{
  jint_t     isr_result;

  uint32     isr_index;

  usb_core_info_type* const 
    core_ptr = &(file_static_vars_ptr->core_table_ptr[core_index]);

  interrupt_resource_info_type** const
    isr_list_ptr = core_ptr->isr_list;

  interrupt_resource_info_type** isr_ptr;

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  for(isr_index = 0, isr_ptr = isr_list_ptr;
      isr_index < core_ptr->num_of_registered_isrs;
      isr_index++, isr_ptr++)
  {
    /* Calling interrupt handler from ISR context */
    HSU_ASSERT((*isr_ptr)->isr_func != NULL);
    isr_result = (*isr_ptr)->isr_func((*isr_ptr)->isr_arg);

    if (isr_result == ISR_CALL_SOFT_INTR)
    {
      HSU_ASSERT((*isr_ptr)->dsr_func != NULL);

      /* As an optimization, the DSR function is directly called
      ** from the interrupt context, since its current implementation allows it.
      ** (i.e., the DSR function does not call any API which is forbidden in the
      ** interrupt context).
      */
      //(*isr_ptr)->dsr_func((*isr_ptr)->dsr_arg);
      dsr_enqueue(core_ptr->hsu_dsr, ((*isr_ptr)->dsr_arg));
    }
  }

}  /* hsu_os_isr_wrapper_int */

/*===========================================================================
FUNCTION HSU_OS_LPM_DSR

DESCRIPTION
  Dsr  function that handles USB deffered lpm services. 
  Calls all the registered service routines

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/


static void hsu_os_lpm_dsr(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[(uint32)payload].usb_lpm_info);

  hsu_lpm_snps_ctx_type *ctx = hsu_lpm_get_snps_ctx();

  //HSU_ULOG(ERROR_MSG, DBG_E_LOG," Entry");

  if(ctx && ctx->crit_sec)
  {
    rex_enter_crit_sect(ctx->crit_sec);
  
    /* Cancel any pending entry of the PHY into low-power mode. */
    core_lpm_info->lpm_entry_is_pending = FALSE;

    if(core_lpm_info->is_in_low_power_mode == TRUE)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG,"exit_lpm_intr");

      /* Un-register the HS-USB interrupt from the Tramp subsystem.
       ** will register it back when it completes the 
       ** low-power mode exit procedure. 
       */
      hsu_os_bus_interrupt_uninit(0);

      hsu_os_bus_exit_lpm_by_intr(0, 0, 0);
    }
    else if(hsu_timer_get_time(&(core_lpm_info->lpm_entry_timer), T_MSEC) > 0)
    {
      if (hsu_port_is_attached(0))
      {
        HSU_ULOG(NO_MSG, BUS_LOG, "cacnel lpm_entry_timer HSU int in LPM pend");
        hsu_timer_cancel(&(core_lpm_info->lpm_entry_timer));
      }
    }

    rex_leave_crit_sect(ctx->crit_sec);
  }
  return;   
}

/*===========================================================================
FUNCTION HSU_OS_DSR

DESCRIPTION
  Dsr  function that handles an USB deffered services. 
  Calls all the registered service routines.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/


static void hsu_os_dsr(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  //HSU_ULOG(ERROR_MSG, DBG_E_LOG," Entry");
  ((jthread_func)ctxt)(payload);
  
  return;   
}


/*===========================================================================
FUNCTION HSU_OS_HS_ISR

DESCRIPTION
  Interrupt wrapper function that handles an USB interrupt. 
  Calls all the registered interrupt service routines and
  their deferred service routines in the order of their registration (using the 
  hsu_os_interrupt_setup function).

  If the core is low-power mode, this function votes against 
  sleeping in the sleep task and brings the core back to an active state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_hs_isr
(
  uint32 core_index,
  pwr_evnt_t pwr_evnt
)
{
  juint8_t  prev_ctx;
  hsu_lpm_snps_ctx_type *lpm_ctx = hsu_lpm_get_snps_ctx();

  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  prev_ctx  = jcurr_ctx;
  jcurr_ctx = ISR_CONTEXT;
  lpm_ctx->is_lpm_voted_by_sw = FALSE;

  if((core_lpm_info->lpm_entry_is_pending == TRUE) ||
     (core_lpm_info->is_in_low_power_mode == TRUE))
  {
    /* Cancel any pending entry of the PHY into low-power mode. */
    core_lpm_info->lpm_entry_is_pending = FALSE;
            
    dsr_enqueue(((usb_core_info_type*)&file_static_vars_ptr->core_table_ptr[core_index])->hsu_lpm_dsr,
               (void *)core_index);

    if(core_lpm_info->is_in_low_power_mode == TRUE)
    {
      /* Restore the original context and return. 
       ** DSR task will complete the low power mode exit procedure
       */       
      jcurr_ctx = prev_ctx;
      return;
    }
  }  

  hsu_os_isr_wrapper_int(core_index);

  /* Restore the original context. */
  jcurr_ctx = prev_ctx;
}  /* hsu_os_hs_isr */

#ifdef FEATURE_HS_USB_SNPS_CORE

void hsu_os_usb3_pwr_evt_irq_wrapper
(
 void
 )
{
  HSU_ULOG_FUNC(BAM_LOG);
  // Use PHY_WAKEUP_EVNT as a generic wakeup event to exit LPM without toggling any flags
  hsu_os_hs_isr(USB3_CORE_INDEX, GENERIC_WAKEUP_EVNT);
}

void hsu_os_usb3_hs_phy_isr_wrapper
(
 void
 )
{
  HSU_ULOG_FUNC(BUS_LOG);
  hsu_os_hs_isr(USB3_CORE_INDEX, PWR_EVNT_SW);
}

/*===========================================================================
FUNCTION HSU_OS_USB3_ISR_WRAPPER

DESCRIPTION
SNPS Core interrupt wrapper function that handles a USB interrupt. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_usb3_isr_wrapper
(
 void
 )
{
  hsu_os_hs_isr(USB3_CORE_INDEX, PWR_EVNT_SW);
}

/*===========================================================================
FUNCTION HSU_OS_BUS_SCHEDULE_LPM_ENTRY_FOR_USB3_PWR_EVT_IRQ

DESCRIPTION
Schedule enter low power mode function and pass pwr evnt as parameter

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_schedule_lpm_entry_for_usb3_pwr_evt_irq(uint32 param)
{  
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[USB3_CORE_INDEX].usb_lpm_info);

  HSU_USE_PARAM(param);
  
  if(core_lpm_info->enter_lpm_handler)
  {
    // Last paramater needs to be set to TRUE to notify that this is
    // power event irq
    core_lpm_info->enter_lpm_handler(USB3_CORE_INDEX, FALSE, (pwr_evnt_t) param);
  }
}

/*===========================================================================
FUNCTION HSU_OS_BUS_SCHEDULE_LPM_EXIT_FOR_USB3_PWR_EVT_IRQ

DESCRIPTION
Schedule exit low power mode function and pass pwr evnt as parameter

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_schedule_lpm_exit_for_usb3_pwr_evt_irq(uint32 param)
{
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[USB3_CORE_INDEX].usb_lpm_info);

  HSU_USE_PARAM(param);
  
  if(core_lpm_info->exit_lpm_handler)
  {
    // Last paramater needs to be set to TRUE to notify that this is
    // power event irq
    core_lpm_info->exit_lpm_handler(USB3_CORE_INDEX, (pwr_evnt_t) param);
  }
}

/*===========================================================================
FUNCTION HSU_OS_HS_ISR_WRAPPER_ASYNC_INTR

DESCRIPTION
  Core 0 (High-Speed core) interrupt wrapper function that handles an 
  USB ASYNC interrupt. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_hs_isr_wrapper_async_intr
(
 void
)
{
  HSU_ULOG_FUNC(BUS_LOG);
  hsu_os_usb3_isr_wrapper();
}

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

/*===========================================================================
FUNCTION HSU_OS_INTERRUPT_SETUP

DESCRIPTION
  Register interrupt service routines for a USB core interrupt.
  Supports chaining of multiple interrupt service routines for the
  same USB core interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns an error code.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_interrupt_setup
(
  /* (IN) Interrupt resource handle previously provided by
  ** the 'hsu_os_bus_alloc_resource' function.
  */
  os_bus_resource_h res,

  /* (IN) User supplied interrupt handler. */
  interrupt_handler isr_func, 

  /* (IN) Argument to be passed to the ISR function. */
  void*             isr_arg,

  /* (IN) User supplied deferred service routine handler. */
  jthread_func      dsr_func,

  /* (IN) Argument to be passed to the DSR function. */
  void*             dsr_arg
)
{
  interrupt_resource_info_type* const
    interrupt_resource_info_ptr = (interrupt_resource_info_type*)res;

  usb_core_info_type*            core_info_ptr = NULL;

  uint32                         core_index;
  interrupt_resource_info_type** isr_list_ptr = NULL;
  uint32                         index_in_isr_list;
  boolean                        dsr_res;

  jresult_t result;



  HSU_PARAM_COULD_BE_CONST(res);

  HSU_MSG_HIGH("hsu_os_interrupt_setup: Entry.");

  HSU_ASSERT(res != NULL);
  HSU_ASSERT(isr_func != NULL);

  core_index = interrupt_resource_info_ptr->core_index;
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_info_ptr = &(file_static_vars_ptr->core_table_ptr[core_index]);

  isr_list_ptr      = core_info_ptr->isr_list;
  index_in_isr_list = core_info_ptr->num_of_registered_isrs;

  HSU_ASSERT(index_in_isr_list < OS_BUS_MAX_ISRS_PER_INTERRUPT);
  HSU_ASSERT(isr_list_ptr[index_in_isr_list] == NULL);

  interrupt_resource_info_ptr->isr_func = isr_func;
  interrupt_resource_info_ptr->isr_arg  = isr_arg;
  interrupt_resource_info_ptr->dsr_func = dsr_func;
  interrupt_resource_info_ptr->dsr_arg  = dsr_arg;
  interrupt_resource_info_ptr->event    = NULL;

  isr_list_ptr[index_in_isr_list] = interrupt_resource_info_ptr;

  core_info_ptr->num_of_registered_isrs++;

  /* If this is the first ISR registered for this USB core's 
  ** interrupt, then also hook the USB core's interrupt
  ** to the appropriate interrupt wrapper function in the AMSS 
  ** TRAMP subsystem. 
  */
  if (core_info_ptr->num_of_registered_isrs == 1)
  {
    result = hsu_os_bus_interrupt_init(core_index);
    if (result != HSU_JSUCCESS)
    {
      HSU_ERR_FATAL("hsu_os_interrupt_setup: Failed to initialize the "
        "interrupts. Result: %d", result, 0, 0);

      /* Roll-back changes. */
      core_info_ptr->num_of_registered_isrs--;

      isr_list_ptr[index_in_isr_list] = NULL;

      interrupt_resource_info_ptr->isr_func = NULL;
      interrupt_resource_info_ptr->isr_arg  = NULL;
      interrupt_resource_info_ptr->dsr_func = NULL;
      interrupt_resource_info_ptr->dsr_arg  = NULL;

      return result;
    }
  }

  dsr_res = dsr_create(&core_info_ptr->hsu_lpm_dsr,
                       hsu_os_lpm_dsr,
                       (void *)core_info_ptr,
                       DSR_PRI_HIGH,
                       DSR_CONTEXT_ISR);
  if(dsr_res == FALSE)
  {
    HSU_ERR_FATAL("hsu_os_interrupt_setup: Failed to initialize the "
          "LPM DSR. Result: %d", dsr_res, 0, 0);

    return -1;
  }

  dsr_res = dsr_create(&core_info_ptr->hsu_dsr,
                       hsu_os_dsr,
                       dsr_func,
                       DSR_PRI_HIGH,
                       DSR_CONTEXT_ISR);
  if(dsr_res == FALSE)
  {
    HSU_ERR_FATAL("hsu_os_interrupt_setup: Failed to initialize the "
          "Generic DSR. Result: %d", dsr_res, 0, 0);

    return -1;
  }
  
  HSU_MSG_HIGH("hsu_os_interrupt_setup: Exit.");

  return HSU_JSUCCESS;

} /* hsu_os_interrupt_setup */


/*===========================================================================
FUNCTION HSU_OS_INTERRUPT_TEARDOWN

DESCRIPTION
  Remove an interrupt handler.

DEPENDENCIES
  Assume here that the unregistered interrupt handler is the last one that
  was registered (using 'hsu_os_interrupt_setup').

RETURN VALUE
  Returns 0 on success, otherwise returns an error code.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_interrupt_teardown
(
  /* (IN) The interrupt resource handle 
  ** (provided by the 'hsu_os_bus_alloc_resource' function).
  */
  os_bus_resource_h res
)
{
  interrupt_resource_info_type* const
    interrupt_resource_info_ptr = (interrupt_resource_info_type*)res;

  usb_core_info_type*             core_info_ptr = NULL;

  uint32                          core_index;
  interrupt_resource_info_type**  isr_list_ptr;
  uint32                          num_of_registered_isrs;
  uint32                          isr_index_to_remove;

  jresult_t result = HSU_JSUCCESS;


  HSU_PARAM_COULD_BE_CONST(res);

  HSU_MSG_HIGH("hsu_os_interrupt_teardown: Entry.");

  HSU_ASSERT(res != NULL);

  core_index = interrupt_resource_info_ptr->core_index;
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_info_ptr          = &(file_static_vars_ptr->core_table_ptr[core_index]);

  isr_list_ptr           = core_info_ptr->isr_list;
  num_of_registered_isrs = core_info_ptr->num_of_registered_isrs;

  isr_index_to_remove = num_of_registered_isrs - 1; 
  
  /* Assert that the unregistered interrupt handler is the 
  ** last one that was registered.
  */
  HSU_ASSERT(isr_list_ptr[isr_index_to_remove] == 
             interrupt_resource_info_ptr);


  core_info_ptr->num_of_registered_isrs--;

  /* If there are no more ISRs registered for this USB core
  ** interrupt, then also un-hook the USB core interrupt
  ** from its interrupt wrapper function in the AMSS TRAMP
  ** subsystem.
  */
  if (core_info_ptr->num_of_registered_isrs == 0)
  {
    result = hsu_os_bus_interrupt_uninit(core_index);
    if (result != HSU_JSUCCESS)
    {
      HSU_ERR_FATAL("hsu_os_interrupt_teardown: Failed to un-initialize the "
        "interrupts. Result: %d", result, 0, 0);
    }
  }

  isr_list_ptr[isr_index_to_remove] = NULL;

  interrupt_resource_info_ptr->isr_func = NULL;
  interrupt_resource_info_ptr->isr_arg  = NULL;
  interrupt_resource_info_ptr->dsr_func = NULL;
  interrupt_resource_info_ptr->dsr_arg  = NULL;

  if(core_info_ptr->hsu_lpm_dsr != NULL)
  {
    dsr_destroy(core_info_ptr->hsu_lpm_dsr);
  }

  if(core_info_ptr->hsu_dsr != NULL)
  {
    dsr_destroy(core_info_ptr->hsu_dsr);
  }      

  HSU_MSG_HIGH("hsu_os_interrupt_teardown: Exit.");

  return result;

} /* hsu_os_interrupt_teardown */


/*===========================================================================
FUNCTION HSU_OS_BUS_SCHEDULE_LPM_ENTRY

DESCRIPTION
  Schedule an LPM entry

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_schedule_lpm_entry(uint32 core_index)
{
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);


  HSU_MSG_HIGH("hsu_os_bus_schedule_lpm_entry: Entry.");

  //Removing conditional statement as part of remote wakeup fix since lpm entry was happening within 1ms.
  /*if (hsu_port_is_attached(core_index))
  {
    if(core_lpm_info->enter_lpm_handler)
    {
      HSU_ULOG(NO_MSG, BUS_LOG, "schedule lpm sw vote");
      core_lpm_info->enter_lpm_handler(core_index, FALSE, PWR_EVNT_SW);
    }
  }
  else
  {*/
    HSU_ULOG(NO_MSG, BUS_LOG, "schedule lpm timer set");
    /* Arm the low power mode entry timer. */
    hsu_timer_set(&(core_lpm_info->lpm_entry_timer),
              core_lpm_info->lpm_entry_timer_value_ms,
              T_MSEC);
    
    core_lpm_info->lpm_entry_is_pending = TRUE;
  //}

  HSU_MSG_HIGH("hsu_os_bus_schedule_lpm_entry: Exit.");

}  /* hsu_os_bus_schedule_lpm_entry */


/*===========================================================================
FUNCTION HSU_OS_BUS_CANCEL_SCHEDULED_LPM_ENTRY

DESCRIPTION
  Cancel any scheduled LPM entry, if exists.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_cancel_scheduled_lpm_entry(uint32 core_index)
{
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);


  HSU_MSG_HIGH("hsu_os_bus_cancel_scheduled_lpm_entry: Entry.");
  HSU_ULOG_FUNC(BUS_LOG);

  /* Cancel any pending entry of the Phy into low-power mode. */
  core_lpm_info->lpm_entry_is_pending = FALSE;
  hsu_timer_cancel(&(core_lpm_info->lpm_entry_timer));

  HSU_MSG_HIGH("hsu_os_bus_cancel_scheduled_lpm_entry: Exit.");

}  /* hsu_os_bus_cancel_scheduled_lpm_entry */


/*===========================================================================
FUNCTION HSU_OS_BUS_SUSPEND

DESCRIPTION
  This function is called when the High-Speed USB
  bus is suspended.

  This function enters the High-Speed USB Phy into low-power mode.
  The actual entry into the low power mode state is delayed using
  a timer in order to enable its cancellation if the bus is resumed
  shortly after returning from this callback.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from 
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
static jresult_t hsu_os_bus_suspend(uint32 core_index, boolean is_in_device_mode)
{
  uint16  dev_event_payload; /*help variable for event_report */    

  /* This variable indicates the number of milliseconds to wait before 
     performing explicit remote wakeup when we are in suspend mode. 
     If its value is set to 0 the device doesn't perform explicit remote wakeup.
  */
  uint32 explicit_remote_wakeup_delay = hsu_conf_sel_get_rm_wakeup_delay();

  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  if (core_index >= NUMBER_OF_CORES)
  {
    HSU_MSG_ERROR_1("hsu_os_bus_suspend: Invalid core index parameter [%d].",
      core_index);
    return JEINVAL;
  }
  
  HSU_MSG_HIGH("hsu_os_bus_suspend() - Entry.");
 
  if (is_in_device_mode)
  {
    HSU_MSG_HIGH("hsu_os_bus_suspend() - Device mode.");
  }
  else
  {
    HSU_MSG_HIGH("hsu_os_bus_suspend() - Host mode.");
  }


  if (core_index == HS_USB_CORE_INDEX)
  {
    /* Cancel any pending cable status check. */
    hsu_timer_cancel(&(core_lpm_info->cable_status_check_timer));

    core_lpm_info->enable_cable_status_check = FALSE;
    HSU_MSG_HIGH("hsu_os_bus_suspend: Cable status check timer canceled.");
  }

  if (is_in_device_mode)
  {
    /* If we are in device mode: arm the remote wakeup timer. */
    if (explicit_remote_wakeup_delay > 0)
    {  
      if (FALSE == core_lpm_info->is_remote_wakeup_pending)
      {
        HSU_ULOG_1(HIGH_MSG, BUS_LOG, "explicit_remote_wakeup_delay %ums", explicit_remote_wakeup_delay);

        core_lpm_info->is_remote_wakeup_pending = TRUE;

        hsu_timer_set(
          &(core_lpm_info->remote_wakeup_timer),
          explicit_remote_wakeup_delay,
          T_MSEC);
      }
    }
    
  
    /* Save the current time stamp */
    core_lpm_info->remote_wakeup_suspend_time_stamp_ms = timetick_get_ms();
  	
    /* Send a notification event */
    dev_event_payload = (uint16)HS_USB_PERIPHERAL_STACK;
    event_report_payload(EVENT_HS_USB_STACK_SUSPENDED ,
                         sizeof(uint16),
                         (void*)(&dev_event_payload) );

  }
  else  /* if (is_in_device_mode) */
  {
    dev_event_payload = (uint16)HS_USB_HOST_STACK;
    event_report_payload(EVENT_HS_USB_STACK_SUSPENDED ,
                         sizeof(uint16),
                         (void*)(&dev_event_payload));

  }  /* if (is_in_device_mode) */
  

  hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_SUSPENDED);

  hsu_os_bus_schedule_lpm_entry(core_index);

  HSU_MSG_HIGH("hsu_os_bus_suspend() - Exit.");
  
  return HSU_JSUCCESS;

} /* hsu_os_bus_suspend */

/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME_COMPLETED

DESCRIPTION
  This function is called when the High-Speed USB bus received a resume 
  notification after it was suspended. The resume sequence is over.
  (The core is resumed)

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/

static void hsu_os_bus_resume_completed(uint32 core_index, 
                                        boolean is_in_device_mode)
{
  uint16  dev_event_payload; /*help variable for event_report */

  /* Send a notification event */
  if (is_in_device_mode)
  {
    dev_event_payload = (uint16)HS_USB_PERIPHERAL_STACK;
    event_report_payload(EVENT_HS_USB_STACK_RESUMED ,
      sizeof(uint16),(void*)(&dev_event_payload) );
  }
  else /*is_in_device_mode*/
  {
    dev_event_payload = (uint16)HS_USB_HOST_STACK;
    event_report_payload(EVENT_HS_USB_STACK_RESUMED ,
      sizeof(uint16),(void*)(&dev_event_payload));
  }

  hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_RESUMED);
}/*hsu_os_bus_resume_completed*/

/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME

DESCRIPTION
  This function is called when the High-Speed USB bus received a resume 
  notification after it was suspended. The resume sequence is not over yet!
  (The core is still suspended)

  Cancel any pending entry of the PHY into low-power mode.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
static jresult_t hsu_os_bus_resume(uint32 core_index, boolean is_in_device_mode)
{
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  HSU_USE_PARAM(core_lpm_info);
  HSU_ULOG_2(HIGH_MSG, BUS_LOG, "os_bus_resume, core %u, dev_mode_b %u", core_index, is_in_device_mode);
  
  /* Cancel any pending entry of the Phy into low-power mode. */
  hsu_os_bus_cancel_scheduled_lpm_entry(core_index);
  HSU_MSG_HIGH("hsu_os_bus_resume: LPM entry was canceled.");

  /* Cancel any pending cable status check. */

  /* TEMP FIX - Sometimes there are false resume events when the phone starts
  ** unconnected. This prevents the LPM entry. The workaround is not to cancel 
  ** the cable status check on resume events.
  ** This is a temp fix until issue B68042 in the Jungo support center
  ** is resolved.
  **
  ** (void)rex_clr_timer(&(core_lpm_info->cable_status_check_timer));
  ** HSU_MSG_HIGH("hsu_os_bus_resume: Cable status check timer canceled.");
  **
  ** TEMP FIX END !!!
  */

  if (is_in_device_mode)
  {
    /* Cancel any pending remote wakeup. */
    HSU_MSG_HIGH("hsu_os_bus_resume: un-scheduling remote wakeup");

    hsu_timer_cancel(&(core_lpm_info->remote_wakeup_timer));

    core_lpm_info->is_remote_wakeup_pending = FALSE;
  }

#if defined (FEATURE_HS_USB_OS_DETECTION)
  /* On USB cable connection if the composition is standard it is switched to the default 
  composition, so that OSes that supports proprietary drivers will be able to enumerate
  the device. This is done only in case we are in device mode. */
  if(is_in_device_mode && core_index == HS_USB_CORE_INDEX)
  {
    const hsu_conf_sel_comp_info_type * cur_comp = hsu_conf_sel_get_cur_comp_details();
    if (cur_comp->supported_drv_by_comp != HSU_SUPP_ALL_OSES_CMP)
    {
      /* Checks if the resume indication occurred due to a composition
      switching or from any other reason */
      if(FALSE == hsu_os_detection_get_resume_initiated_by_comp_switch())
      {
        if ((cur_comp->supported_drv_by_comp == HSU_STANDARD_CMP_ONLY) && 
          (hsu_os_detection_get_supported_drv() != STANDARD_DRV_ONLY)) 
        {
          /* On cable connection event if the current composition is standard-
          switch to proprietary composition  */
          hsu_os_detection_set_timer(TRUE);
        }
      } 
      else /* resume_initiated_by_comp_switch == TRUE */
      {
        hsu_os_detection_set_resume_initiated_by_comp_switch(FALSE);
      }
    }
  } /* is_in_device_mode && core_index == HS_USB_CORE_INDEX*/
#endif /* FEATURE_HS_USB_OS_DETECTION */

#ifdef FEATURE_HS_USB_RESET_BAM_UPON_RESUME_BEFORE_EP_PRIME
    hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_RESUME_BEGIN);
#endif /* FEATURE_HS_USB_RESET_BAM_UPON_RESUME_BEFORE_EP_PRIME */

  return HSU_JSUCCESS;

} /* hsu_os_bus_resume */


/*===========================================================================
FUNCTION HSU_OS_BUS_CONNECT

DESCRIPTION
This function is called when the High-Speed USB bus received a connect 
notification. 

DEPENDENCIES
The connect event is received only when the stack is configured to
work in non-OTG mode.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_bus_connect(uint32 core_index)
{
  HSU_MSG_HIGH("hsu_os_bus_connect() - Entry.");
  if (HSU_CORE_IDX_PRIMARY != OS_BUS_TO_HAL_CORE_ID(core_index))
  {
    core_allow_enumeration(OS_BUS_TO_HAL_CORE_ID(core_index));
  }

  /* Only applicable for stack configured as device only */
  hsu_al_task_enqueue_cmd(hsu_conf_sel_stack_utils_device_restart_core, (void *)core_index);
  hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_CONNECTED);

  HSU_MSG_HIGH("hsu_os_bus_connect() - Exit.");

}  /* hsu_os_bus_connect */


/*===========================================================================
FUNCTION HSU_OS_BUS_DISCONNECT

DESCRIPTION
  This function is called when the High-Speed USB bus received a disconnect 
  notification after it was suspended.

DEPENDENCIES
  The disconnect event is received only when the stack is configured to
  work in non-OTG mode.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_disconnect(uint32 core_index, boolean restart_core)
{

  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  HSU_MSG_HIGH("hsu_os_bus_disconnect() - Entry.");

  if (core_index == HS_USB_CORE_INDEX)
  {
    /* Cancel any pending cable status check. */
    hsu_timer_cancel(&(core_lpm_info->cable_status_check_timer));
    
    core_lpm_info->enable_cable_status_check = FALSE;
    HSU_MSG_HIGH("hsu_os_bus_disconnect() - Cable status check timer"
                  " was canceled.");
  }
  
  #if defined (FEATURE_HS_USB_OS_DETECTION)
  if (core_index == HS_USB_CORE_INDEX)
  {
    /* Cancel any pending OS detection timer and reset parameters */
    hsu_os_detection_clr_timer();
    hsu_os_detection_reset_params();
  }
  #endif /* FEATURE_HS_USB_OS_DETECTION */

  /* Arm the low power mode entry timer. */
  HSU_MSG_HIGH("hsu_os_bus_disconnect() - scheduling LPM entry");

  /* Schedule LPM upon disconnect only for HSUSB. 
     For HSIC, cases of context switching was seen during the time of restart core leading to 
     artificial delays in execution causing enumeration issues */
  // Stop the RUN/STOP bit because the controller cannot observe VBUS disconnect anymore.
  core_disallow_enumeration(OS_BUS_TO_HAL_CORE_ID(core_index));

  hsu_os_bus_schedule_lpm_entry(core_index);

  #ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC
  /* Core should be restarted, only for a physical USB cable disconnection, 
     and not for disconnect event which was triggered by spoof_disconnect. 
     This is because spoof_disconnect itself is used in restart core procedure. */
  if (restart_core)
  {
    if (file_static_vars_ptr->core_table_ptr[core_index].controller_info_ptr)
    {
      hsu_al_task_enqueue_cmd(hsu_conf_sel_stack_utils_device_restart_core, (void *)core_index);
    }
  }
  #endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */
  

  /* Only applicable for stack configured as device only */
  hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_DISCONNECTED);
  HSU_USE_PARAM(restart_core);
  HSU_MSG_HIGH("hsu_os_bus_disconnect() - Exit.");

} /* hsu_os_bus_disconnect */


/*===========================================================================
FUNCTION HSU_OS_NOTIFY

DESCRIPTION
  This function is called by the High-Speed USB core controller
  driver for notifying about bus events.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_notify(jevent_t event, void * param)
{
  #define LOW_WORD_MASK  0x0000FFFF
  #define HIGH_WORD_MASK 0xFFFF0000

  /* Core index is passed in low word of param */
  uint32     core_index = OS_BUS_FROM_HAL_CORE_ID((uint32)param & LOW_WORD_MASK);
  jresult_t  return_value;
  
  hsu_lpm_info_type* core_lpm_info = 
      &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  timetick_type remaining_time  = 0; 
  timetick_type elapsed_time    = 0;
  timetick_type current_time_ms = 0;

#if defined(FEATURE_HS_USB_POLLING)
  if(hsu_polling_is_active()) {return 0;}
#endif /* FEATURE_HS_USB_POLLING */

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  /* 
  ** For now, power management is not supported for 
  ** the Full-Speed core. 
  ** TODO - Update this function when Full-Speed core power management 
  ** is implemented. Remember to take into account that the second core
  ** may be also a High-Speed core (use the 'core type' field).
  */
  HSU_ULOG_ENUM_1(HIGH_MSG, BUS_LOG, "hsu_os_notify_entry, core_index %u", event, HSU_BUS_NOTIFY_OFFSET2, core_index);

  switch (event)
  {
  case NOTIFY_DEVICE_SUSPEND:
    if (TRUE == device_resumed[core_index])
    {
      /* filter the event for only valid suspend */      
      return_value = hsu_os_bus_suspend(core_index, TRUE);
      device_resumed[core_index] = FALSE;
      hsu_event_log(EVT_BUS_SUSPEND, OS_BUS_TO_HAL_CORE_ID(core_index));
    } 
    else 
    {
      return_value = HSU_JSUCCESS;
    }
    break;

  #ifdef CONFIG_JHOST 
  case NOTIFY_HOST_SUSPEND:
    return_value = hsu_os_bus_suspend(core_index, FALSE);
    break;
  #endif  /* CONFIG_JHOST */
    
  case NOTIFY_DEVICE_RESUME:
    return_value = hsu_os_bus_resume(core_index, TRUE);
    device_resumed[core_index] = TRUE;
    hsu_event_log(EVT_BUS_RESUME, OS_BUS_TO_HAL_CORE_ID(core_index));
    break;

  case NOTIFY_DEVICE_RESUME_COMPLETED:
    hsu_os_bus_resume_completed(core_index, TRUE);
    return_value = HSU_JSUCCESS;
    break;

  #ifdef CONFIG_JHOST
  case NOTIFY_HOST_RESUME:
    /* hsu_os_bus_resume() should be invoked before the call to exit_lpm
    ** in order to make sure that any pending LPM enrty will be cancelled.
    ** Otherwise, the LPM entry timer can expire after exit LPM begins, 
    ** resulting in entering LPM after exit LPM finishes. This is illegal 
    ** because host mode applications assume we are not in LPM after 
    ** requesting resume from the USB stack.
    */
    return_value = hsu_os_bus_resume(core_index, FALSE);

    if (core_lpm_info->exit_lpm_handler)
    {
      core_lpm_info->exit_lpm_handler(core_index, PWR_EVNT_SW);
    }

    /* In host mode RESUME_COMPLETED is missing so we need to 
    ** call hsu_os_bus_resume_completed in order to notify the applications
    */
    hsu_os_bus_resume_completed(core_index, FALSE);

    break;
  #endif  /* CONFIG_JHOST */

  case NOTIFY_DEVICE_REMOTE_WAKEUP:
    if ((core_lpm_info->port_is_attached != NULL) &&
        FALSE == core_lpm_info->port_is_attached(core_index))
    {
      HSU_MSG_HIGH("hsu_os_notify(REMOTE_WAKEUP): Port is not attached. "
                   "Denying remote wakeup.");

      return_value = JEPERM;
    }
    else
    {
      /* Cancel any pending LPM entry. */
      hsu_os_bus_cancel_scheduled_lpm_entry(core_index);
      HSU_MSG_HIGH("hsu_os_notify(REMOTE_WAKEUP) - port attached, lpm canceled");

      if (core_lpm_info->exit_lpm_handler)  
      {           
        core_lpm_info->was_lpm_exit_due_to_remote_wakeup = TRUE;
        core_lpm_info->exit_lpm_handler(core_index, PWR_EVNT_SW);
      }

      /* prevent remote wakeup to be issued too soon. Insure that at least 
         OS__REMOTE_WAKEUP_MIN_DELAY_MS have passed from suspend (spec defines 5ms).
         If previous suspend was due to cable disconnect, this delay will allow b_sess_vld
         to go low and allow us to correctly identify cable not connected */
      current_time_ms = timetick_get_ms();
     
      /* Calculate elapsed time since last suspend */
      if (core_lpm_info->remote_wakeup_suspend_time_stamp_ms 
          <= current_time_ms)
      {
       /* Time tick could have wrapped around.  
        * Normally, it should be less or equal to current time 
        */
        elapsed_time = 
          current_time_ms - core_lpm_info->remote_wakeup_suspend_time_stamp_ms;
      }
      else
      {
        /* Time tick has wrapped around.
         * In this case, there is not easy way to determine how much time has
         * elapsed due to wraparound of the timetick value.
         * Conservatively, set suspend time to current time to make it appear
         * that 0ms has elapsed.
         * Logic will enforce OS_REMOTE_WAKEUP_MIN_DELAY_MS gap by scheduling
         * the timer to generate REMOTE_WAKEUP even again.
         */
        elapsed_time = 0;
        core_lpm_info->remote_wakeup_suspend_time_stamp_ms = current_time_ms;

        HSU_ULOG_1(NO_MSG, BUS_LOG, "suspend_to_remote_wakeup_elapsed_time, xcp_tick_wrapped_around, time_stamp %ums",
          core_lpm_info->remote_wakeup_suspend_time_stamp_ms);
      }

      HSU_ULOG_1(NO_MSG, BUS_LOG, "suspend_to_remote_wakeup_elaspsed_time %u", elapsed_time);

      /* Check if elapsed time is less that minimal delay between SUSPEND and REMOTE WAKEUP 
      */
      if (elapsed_time < OS_REMOTE_WAKEUP_MIN_DELAY_MS)
      {
       /* Calculate remining time
        */
        remaining_time = OS_REMOTE_WAKEUP_MIN_DELAY_MS - elapsed_time;
        
        HSU_ULOG_2(HIGH_MSG, BUS_LOG, "remote_wakup_min_delay_timer_scheduled, remaining_time %u, min_delay %u", remaining_time, OS_REMOTE_WAKEUP_MIN_DELAY_MS);      

        core_lpm_info->is_remote_wakeup_pending = TRUE;

        /* Schedule timer for remaining time */
        hsu_timer_set(
          &(core_lpm_info->remote_wakeup_timer),
          remaining_time,
          T_MSEC);    

        return_value = JEAGAIN;

        /* Exit out of switch statement */         
      }
      else
      {
        hsu_event_log(EVT_REMOTE_WAKEUP, core_index);

        // TODO: --- Akash --- Why is there lpm_entry?
        #ifdef CI_MERGE
          hsu_os_bus_schedule_lpm_entry(core_index); 
        #endif
        return_value = HSU_JSUCCESS;
      }
    }
    break;

  #ifdef CONFIG_JOTG
  case NOTIFY_OTG_BUSY:
    return_value = hsu_os_bus_resume(core_index, FALSE);
    /*In host mode RESUME_COMPLETED is missing so we need to 
    call hsu_os_bus_resume_completed in order to notify the applications*/
    hsu_os_bus_resume_completed(core_index, FALSE);
    break;

  case NOTIFY_OTG_IDLE:
    return_value = hsu_os_bus_suspend(core_index, FALSE);
    break;
  #endif  /* CONFIG_JOTG */

  #ifdef CONFIG_JHOST

  #endif  /* CONFIG_JHOST */

  case NOTIFY_DEVICE_CONFIGURED:
    #if defined (FEATURE_HS_USB_OS_DETECTION)
    {
      const hsu_conf_sel_comp_info_type* cur_comp = hsu_conf_sel_get_cur_comp_details();
      if (cur_comp->supported_drv_by_comp != HSU_SUPP_ALL_OSES_CMP)
      {
        hsu_os_detection_handle_set_config();
      }
    }
    #endif /* FEATURE_HS_USB_OS_DETECTION */

    hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_CONFIGURED);

#ifndef USB_THREADX
#ifndef FEATURE_HS_USB_PRE_SILICON_BU
    tracer_event_simple(HSU_USB_CONFIGURED); 
#endif // FEATURE_HS_USB_PRE_SILICON_BU
#endif
    
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_UNCONFIGURED:
    hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_UNCONFIGURED);
    #if defined (FEATURE_HS_USB_OS_DETECTION)
    if (hsu_os_detection_get_comp_switch_is_done() == TRUE)
    {
      /* Disconnect event - cancel any pending OS detection timer and reset parameters */
      hsu_os_detection_clr_timer();
      hsu_os_detection_reset_params();
    }
    else
    {
      /* Cancel any pending OS detection timer */
      hsu_os_detection_clr_timer();
    }
    #endif /* FEATURE_HS_USB_OS_DETECTION */

    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_CONNECT:
    hsu_os_bus_connect(core_index);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_DISCONNECT:
    {
      /* A boolean flag is passed in high word of param */
      uint16 restart_core = (uint16)(((uint32)param & HIGH_WORD_MASK) >> 16);
      hsu_os_bus_disconnect(core_index, (boolean)restart_core);
    }
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_RESET:
    HSU_MSG_LOW("hsu_os_notify: NOTIFY_DEVICE_RESET");
    hsu_event_log(EVT_BUS_RESET, OS_BUS_TO_HAL_CORE_ID(core_index));
    hsu_bus_mode_changed(OS_BUS_TO_HAL_CORE_ID(core_index), HSU_BUS_MODE_RESET);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_SPEED_CHANGE:
    {
#ifdef FEATURE_HS_USB_BAM
      hsu_al_bam_utils_ipa_notify_core_speed();
#endif
      return_value = HSU_JSUCCESS;
    }
    break;

#ifdef FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
  case NOTIFY_DEVICE_SPOOF_DISCONNECT:    
    hsusb_vregmgr_vbus_callback(0);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_SPOOF_CONNECT:
    hsusb_vregmgr_vbus_callback(1);
    return_value = HSU_JSUCCESS;
    break;
#endif // FEATURE_HS_USB_TEST_SPOOF_DISCONNECT

  default:
    return_value = HSU_JSUCCESS;
    break;
  }
  
  HSU_MSG_HIGH("hsu_os_notify: Exit.");
  return return_value;
}  /* hsu_os_notify */


/*
 * Temporary wrapper function for hsu_os_notify. DO NOT USE.
 */
jresult_t os_notify(jevent_t event, void * param)
{
  return hsu_os_notify(event, param);
}


/*===========================================================================
FUNCTION HSU_OS_NOTIFY_STACK_READY

DESCRIPTION
  Tell the OS abstraction layer that the USB stack is fully initialized.
  This notification is used for suspending the USB bus and for power management.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_notify_stack_ready(void)
{
  /* Schedule a USB cable status check. */
  int i;
  hsu_lpm_info_type* core_lpm_info;

  for (i = 0; i < NUMBER_OF_CORES; i++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(i)) == NULL)
    {
      continue;
    }

    core_lpm_info = &(file_static_vars_ptr->core_table_ptr[i].usb_lpm_info);
    if (i == HS_USB_CORE_INDEX)
    {    
      hsu_timer_set(
        &(core_lpm_info->cable_status_check_timer),
        core_lpm_info->lpm_entry_timer_value_ms,
        T_MSEC);  
      
      core_lpm_info->enable_cable_status_check = TRUE;
    }
  }
}  /* hsu_os_notify_stack_ready */


/*===========================================================================
FUNCTION HSU_OS_HW_INIT

DESCRIPTION
  Discover all controllers on bus and call add_generic_controller
  for each.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_hw_init()
{
  uint8 core_index;
  jresult_t  return_value = HSU_JSUCCESS;

  HSU_MSG_HIGH("hsu_os_hw_init: Entry.");

  /* jsafe_enter needs to be called, because this function
  ** calls echi_init later on. ehci_init uses the hsu_os_delay_ms function,
  ** which requires that the Giant mutex will be locked before calling it.
  */
  jsafe_enter();

  for (core_index = 0;
       core_index < NUMBER_OF_CORES;
       core_index++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }

    return_value = hsu_os_core_hw_init(OS_BUS_TO_HAL_CORE_ID(core_index));
    if (return_value != HSU_JSUCCESS)
    {
      break;
    }
  }  /* for (core_index ...) */

    jsafe_leave();

  HSU_MSG_HIGH("hsu_os_hw_init: Exit.");

  return return_value;

} /* hsu_os_hw_init */


/*===========================================================================
FUNCTION HSU_OS_CORE_HW_INIT

DESCRIPTION
  call add_generic_controller for the core specified by core_index param.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_core_hw_init(uint32 core_index)
{
  usb_core_info_type* const 
  core_table_ptr= file_static_vars_ptr->core_table_ptr;

  jresult_t return_value;

  hsu_core_config_init_mode_enum stack_init_mode;

  if (core_index >= NUMBER_OF_CORES)
  {
    HSU_MSG_ERROR_1("hsu_os_core_hw_init: Invalid core index parameter [%d].",
                  core_index);
    return JEINVAL;
  }

  HSU_MSG_HIGH_1("hsu_os_core_hw_init: HS_USB controller registration for core %d is starting...",
    core_index);

  if (core_table_ptr[core_index].controller_info_ptr)
  {
    HSU_MSG_HIGH_1("hsu_os_core_hw_init: HS_USB controller registration for core %d is already done.",
      core_index);
    return HSU_JSUCCESS;
  }

  stack_init_mode = hsu_core_config_get_stack_init_mode(
    OS_BUS_TO_HAL_CORE_ID(core_index));
  if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_DEVICE)
  {
    /* only the device entry in the controllers_list should be added. */
    #if defined FEATURE_HS_USB_SNPS_CORE
    if (core_index == USB3_CORE_INDEX)
    {
      core_table_ptr[core_index].controller_info_ptr = 
        &(core_table_ptr[core_index].controllers_list[0]);
    }
    #endif  /* FEATURE_HS_USB_SNPS_CORE */

  }
  else if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_OTG)
  {
    /* The OTG entry (or device entry for the full-speed core) 
    ** in the controllers_list should be added.
    */
    core_table_ptr[core_index].controller_info_ptr = 
      &(core_table_ptr[core_index].controllers_list[0]);
  }
  else if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_LAST)
  {
    HSU_MSG_HIGH_1("hsu_os_core_hw_init: HS_USB controller registration skipped, core %d is NULL.",
      core_index);
    return HSU_JSUCCESS;
  }
  else
  {
    HSU_ERR_FATAL("hsu_os_hw_init(): *_get_primary_stack_init_mode returned "
      "an undefined init option.", 0, 0, 0);
    return EUNKNOWN;
  }
  
  return_value = 
    add_generic_controller((os_bus_h)(uint32)OS_BUS_TO_HAL_CORE_ID(core_index),
                            (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type,
                            &(core_table_ptr[core_index].controller_info_ptr->controller_handle));

  if (return_value != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR_3("hsu_os_core_hw_init: HS-USB controller registration for core %d failed. Type: %d, Result: %d.",
      core_index, (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type, return_value);
  }
  else
  {
    HSU_MSG_HIGH_2("hsu_os_core_hw_init: HS_USB controller registration for core %d succeeded. Type: %d",
      core_index, (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type);
  }

  return return_value;

} /* hsu_os_core_hw_init */


/*===========================================================================
FUNCTION HSU_OS_HW_UNINIT

DESCRIPTION
  Remove all controllers, call del_generic_controller for each.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_hw_uninit(void)
{
  uint8 core_index;

  HSU_MSG_HIGH("hsu_os_hw_uninit: Un-registering High-Speed USB controllers.");

  for (core_index = 0; core_index < NUMBER_OF_CORES; core_index++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }

    hsu_os_core_hw_uninit(core_index);
  }

  HSU_MSG_HIGH("hsu_os_hw_uninit: HS-USB controllers' un-registration is completed.");

} /* hsu_os_hw_uninit */

/*===========================================================================
FUNCTION HSU_OS_CORE_HW_UNINIT

DESCRIPTION
  Remove a specific controller, call del_generic_controller to delete it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_core_hw_uninit(uint32 core_index)
{
  usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;

  if (core_index >= NUMBER_OF_CORES)
  {
    HSU_MSG_ERROR_1("hsu_os_core_hw_uninit: Invalid core index parameter [%d].",
                  core_index);
    return;
  }
  
  HSU_MSG_HIGH_1("hsu_os_core_hw_uninit: HS_USB controller un-registration for core %d is starting...",
    core_index);

  if ( (core_table_ptr[core_index].controller_info_ptr)
    && (core_table_ptr[core_index].controller_info_ptr->controller_handle) )
  {
    jresult_t result = del_generic_controller(core_table_ptr[core_index].controller_info_ptr->controller_handle);

    if (result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR_2("hsu_os_core_hw_uninit: HS-USB controller un-registration for core %d failed. Handle: %p.",
        core_index, core_table_ptr[core_index].controller_info_ptr->controller_handle);
    }
    else
    {
      HSU_MSG_HIGH_2("hsu_os_core_hw_uninit: HS_USB controller un-registration for core %d succeeded. Type: %d",
        core_index, (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type);
    }

    HSU_ASSERT(result == HSU_JSUCCESS);

    core_table_ptr[core_index].controller_info_ptr = NULL;
  }
  else
  {
    HSU_MSG_HIGH_1("hsu_os_core_hw_uninit: HS_USB controller un-registration for core %d is already done.",
      core_index);
  }
} /* hsu_os_core_hw_uninit */

#if (defined(FEATURE_HS_USB_POLLING) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)) \
   || defined(FEATURE_HS_USB_BRINGUP_POLL_INTR)
/*===========================================================================
FUNCTION HSU_OS_BUS_POLL_INTERRUPT

DESCRIPTION
  Checks whether a High-Speed USB controller interrupt
  has occurred.

  This function accesses High-Speed USB controller interrupt status registers
  in order to determine whether an interrupt has occurred.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an interrupt has occurred. FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_poll_interrupt(void)
{
  return hsu_os_bus_poll_interrupt_idx(HS_USB_CORE_INDEX);
}  /* hsu_os_bus_poll_interrupt */

/*===========================================================================
FUNCTION HSU_OS_BUS_POLL_INTERRUPT_IDX

DESCRIPTION
  Checks whether a High-Speed USB controller interrupt
  has occurred on a particular core index.

  This function accesses High-Speed USB controller interrupt status registers
  in order to determine whether an interrupt has occurred.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an interrupt has occurred. FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_poll_interrupt_idx(uint32 logical_core_index)
{
  const uint32 usbsts_reg_offset  = 0x144;
  const uint32 usbintr_reg_offset = 0x148;
  const uint32 otgsc_reg_offset   = 0x1A4;

  /* Pointer to the the memory resource of the HS-USB core(OTG) */
  bus_resource_type* const bus_resource = 
    &(file_static_vars_ptr->core_table_ptr[logical_core_index].controllers_list[0].resource_list[1]);
  memory_resource_info_type* const mem_res_ptr = 
    &(bus_resource->resource_instance.mem_resource_instance);

  boolean  return_value;

  uint32   usbsts_reg_value;
  uint32   usbintr_reg_value;
  uint32   otgsc_reg_value;

  uint32   usbsts_reg_set_interrupt_bits;
  uint32   otgsc_reg_set_interrupt_bits;

  /* Read the enabled interrupts bit mask. */
  usbintr_reg_value = hsu_os_bus_space_read_4(mem_res_ptr,
                                          usbintr_reg_offset);

  /* Read the recent USB controller interrupt events. */
  usbsts_reg_value  = hsu_os_bus_space_read_4(mem_res_ptr,
                                          usbsts_reg_offset);

  usbsts_reg_set_interrupt_bits = usbsts_reg_value & usbintr_reg_value;

  /* Read the enabled interrupts bit mask & interrupts status from OTGSC 
     register */
  otgsc_reg_value = hsu_os_bus_space_read_4(mem_res_ptr, otgsc_reg_offset);

  /* OTGSC bits [31..24] are interrupt enable bits, and bits [23..16] are 
    interrupt status bits. */
  otgsc_reg_set_interrupt_bits = (otgsc_reg_value >> 24) & (otgsc_reg_value >> 16);

  if (HS_USB_CORE_INDEX == logical_core_index)
  {
    /* Return TRUE if and only if identified an interrupt in USBSTS or OTGSC */
    if ((usbsts_reg_set_interrupt_bits != 0) || (otgsc_reg_set_interrupt_bits != 0))
    {
      return_value = TRUE;
    }
    else
    {
      return_value = FALSE;
    }
  }
  else
  {
    return_value = FALSE;
    HSU_ERR_FATAL("unknown logical core index (%u)", logical_core_index, 0, 0);
  }
  return return_value;
}
#endif  /*(FEATURE_HS_USB_POLLING && !FEATURE_HS_USB_NO_HS_OTG_CORE) ||
          (FEATURE_HS_USB_BRINGUP_POLL_INTR) */

#if (defined(FEATURE_HS_USB_POLLING) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)) \
   || defined(FEATURE_HS_USB_BRINGUP_POLL_INTR)
/*===========================================================================
FUNCTION HSU_OS_BUS_HANDLE_INTERRUPT

DESCRIPTION
  This function handles a High-Speed USB OTG core interrupt.
  This function is used during the phone software download mode.

  NOTE - This function should only be called when a High-Speed USB OTG
         core interrupt is detected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_handle_interrupt(void)
{
  hsu_os_isr_wrapper_int(HS_USB_CORE_INDEX);
}  /* hsu_os_bus_handle_interrupt */

/*===========================================================================
FUNCTION HSU_OS_BUS_HANDLE_INTERRUPT_IDX

DESCRIPTION
  This function handles a USB or HSIC core interrupt.
  This function is used during the phone software download mode.

  NOTE - This function should only be called when a High-Speed USB OTG
         or HSIC core interrupt is detected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_handle_interrupt_idx(uint32 logical_core_index)
{
  if (HS_USB_CORE_INDEX == logical_core_index)
  {
    hsu_os_isr_wrapper_int(HS_USB_CORE_INDEX);
  }
  else
  {
    HSU_ERR_FATAL("unknown logical core index (%u)", logical_core_index, 0, 0);
  }
}
#endif  /* (FEATURE_HS_USB_POLLING && !FEATURE_HS_USB_NO_HS_OTG_CORE) ||
          (FEATURE_HS_USB_BRINGUP_POLL_INTR) */


/*===========================================================================
FUNCTION HSU_OS_BUS_INIT

DESCRIPTION
  Initializes the bus module. Called by the OS abstraction layer's main
  initialization function (hsu_os_init).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_init(void)
{
  hsu_lpm_info_type* core_lpm_info;
#ifdef FEATURE_HS_USB_F3
  hsu_platform_info_type const *platform_info;
#endif /* FEATURE_HS_USB_F3 */
  uint32 core_index;

  HSU_MSG_HIGH("hsu_os_bus_init: Entry.");

  HAL_hsusb_Init();

#ifndef FEATURE_HS_USB_PRE_SILICON_BU
  // TODO: CHECK LOGIC: HSU_ASSERT(HAL_hsusb_SupportedCoresQty() >= NUMBER_OF_CORES);
#endif

  hsu_os_bus_init_core_index_maps();
 
  for (core_index = 0;
  core_index < NUMBER_OF_CORES;
  core_index++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }

    core_lpm_info =
      &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

    core_lpm_info->remote_wakeup_timer = hsu_timer_init(
      HSU_TIMER_CID__RM_WAKEUP,
      HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
      hsu_os_bus_rm_wakeup_timer_cb,
      (timer_cb_data_type)core_index);

    core_lpm_info->lpm_entry_timer = hsu_timer_init(
      HSU_TIMER_CID__LPM_ENTRY,
      HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
      hsu_os_bus_lpm_entry_timer_cb,
      (timer_cb_data_type)core_index);

    if (core_index == HS_USB_CORE_INDEX)
    {
      core_lpm_info->cable_status_check_timer = hsu_timer_init(
        HSU_TIMER_CID__CABLE_STATUS_CHECK,
        HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
        hsu_os_bus_cable_status_check_timer_cb,
        (timer_cb_data_type)core_index);
    }

    file_static_vars_ptr->core_table_ptr[core_index].resume_peripherals_timer = hsu_timer_init(
      HSU_TIMER_CID__RESUME_PERIPHERALS,
      HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
      hsu_os_bus_resume_peripherals_timer_cb,
      (timer_cb_data_type)core_index);

    device_resumed[core_index] = TRUE;
  }
  rex_init_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
  file_static_vars_ptr->usb_reset_rework_is_installed = 
    hsu_api_usb_reset_rework_installed();
  #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */


#ifdef  FEATURE_HS_USB_F3
  platform_info = hsu_platform_get_info();

#ifdef CI_MERGE
  if(platform_info->gpio_hsic_ready)
  {
    hsu_al_task_enqueue_cmd(hsu_os_ap2mdm_status_gpio_register, (void *)platform_info);
  }
  else
  {
    HSU_ULOG(NO_MSG, DATA_LOG, "gpio_hsic_ready_not_configured");
  }
#endif
  /* Register for VDD MIN in the AL task */

  if((platform_info->gpio_mdm2apq_vdd_min)&&(platform_info->gpio_apq2mdm_vdd_min))
  {
    hsu_al_task_enqueue_cmd(hsu_os_vddmin_gpio_register, (void *)platform_info);
  }
  else
  {
    HSU_ULOG(NO_MSG, DATA_LOG, "gpio_vdd_logging_not_configured");
  }
#endif /* FEATURE_HS_USB_F3 */

  HSU_MSG_HIGH("hsu_os_bus_init: Exit.");

  return HSU_JSUCCESS;

} /* hsu_os_bus_init */


/*===========================================================================
FUNCTION HSU_OS_BUS_UNINIT

DESCRIPTION
  Un-initialize the bus module. Called by the OS abstraction layer's main
  un-initialization function (hsu_os_uninit).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_uninit(void)
{
  HSU_MSG_HIGH("hsu_os_bus_uninit: Entry.");

  // No-op

  HSU_MSG_HIGH("hsu_os_bus_uninit: Exit.");

} /* hsu_os_bus_uninit */


/*===========================================================================
FUNCTION HSU_OS_BUS_ALLOC_RESOURCE

DESCRIPTION
  Returns a handle to a bus resource.

DEPENDENCIES
  hsu_os_bus_init must be called prior to calling this function.

RETURN VALUE
  Returns a handle to the allocated resource, 
  or NULL if the allocation fails.

SIDE EFFECTS
  None.
===========================================================================*/
os_bus_resource_h hsu_os_bus_alloc_resource
(
  /* High-Speed USB core handle 
  ** (provided by the 'add_generic_controller' function).
  */
  os_bus_h bus,

  /* Resource type.
  ** May be one of the following types: 
  **   SYS_RES_IRQ    - Interrupt line.
  **   SYS_RES_MEMORY - Physical address range.
  */
  jint_t type,

  /* Resource id.
  */
  jint_t id
)
{
  uint32  core_index,
          controller_index,
          resource_index;

  os_bus_resource_h return_value = NULL;

  bus_resource_type*
    resource_list_ptr = NULL;

  usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;


  HSU_PARAM_COULD_BE_CONST(bus);

  core_index = OS_BUS_FROM_HAL_CORE_ID((uint32)bus);
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  /* Only memory or interrupt resources are supported. */
  HSU_ASSERT((type == SYS_RES_MEMORY) ||
             (type == SYS_RES_IRQ));

  HSU_MSG_HIGH_2("hsu_os_bus_alloc_resource: Allocating %s resource of HS-USB core %d",
               (type == SYS_RES_IRQ) ? "an interrupt" : "a memory",
                core_index);

  for (controller_index = 0;
       controller_index < core_table_ptr[core_index].number_of_controllers;
       controller_index++)
  {
  /* Scan for the resource whose type is 'type'.
  */
    resource_list_ptr = core_table_ptr[core_index].
                          controllers_list[controller_index].resource_list;

  for (resource_index = 0;
       resource_index < OS_BUS_NUM_RES_PER_CONTROLLER;
       resource_index++)
  {
    if ((type != resource_list_ptr[resource_index].resource_type) ||
        (id != resource_list_ptr[resource_index].resource_id))
    {
      continue;
    }

    /* The requested resource was found.
    ** Return a pointer to this resource.
    */
    return_value = (os_bus_resource_h)
        &(resource_list_ptr[resource_index].resource_instance);

    /* Quit the for loop. */
    break;
  }  /* for (resource_index ...) */
  }  /* for (controller_index...) */

  /* Verify that the requested resource was
  ** found in the database.
  */
  HSU_ASSERT(return_value != NULL);

  HSU_MSG_HIGH("hsu_os_bus_alloc_resource: Resource allocation is successful.");

  return return_value;

} /* hsu_os_bus_alloc_resource */


/*===========================================================================
FUNCTION HSU_OS_BUS_RELEASE_RESOURCE

DESCRIPTION
  Releases a handle to a bus resource, acquired using
  hsu_os_bus_alloc_resource().

  Actually, this function does nothing but to assert that this
  resource handle refers to a valid resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_release_resource
(
  /* A High-Speed USB core handle previously registered
  ** at the stack by 'add_generic_controller'.
  */
  os_bus_h          bus,

  /* The handle of the released resource.
  ** (Previously supplied by 'hsu_os_bus_alloc_resource').
  */
  os_bus_resource_h res
)
{
  uint32  core_index,
          controller_index,
          resource_index;

  bus_resource_type*
    resource_list_ptr = NULL;

  boolean resource_found;

  usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;


  HSU_PARAM_COULD_BE_CONST(bus);
  HSU_PARAM_COULD_BE_CONST(res);

  core_index = OS_BUS_FROM_HAL_CORE_ID((uint32)bus);
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  HSU_MSG_HIGH_2("hsu_os_bus_release_resource: Releasing HS-USB core %d resource. Handle: %p",
                core_index, res);

  /* Scan for the resource. */
  resource_found = FALSE;

  for (controller_index = 0;
       controller_index < core_table_ptr[core_index].number_of_controllers;
       controller_index++)
  {
    /* Scan for the resource whose type is 'type'.
    */
    resource_list_ptr = core_table_ptr[core_index].
                          controllers_list[controller_index].resource_list;

  for (resource_index = 0;
       resource_index < OS_BUS_NUM_RES_PER_CONTROLLER;
       resource_index++)
  {
      if (res != &(resource_list_ptr[resource_index].resource_instance))
    {
      continue;
    }

    /* The requested resource was found.
    */
    resource_found = TRUE;

    /* Quit the for loop. */
    break;
  }  /* for (resource_index ...) */
  }  /* for (controller_index ...) */

  if (resource_found)
  {
    HSU_MSG_HIGH("hsu_os_bus_release_resource: Resource release is successful.");
  }
  else
  {
    HSU_ERR_FATAL("hsu_os_bus_release_resource: Resource release has failed.",
      0, 0, 0);
  }

} /* hsu_os_bus_release_resource */


/*===========================================================================
FUNCTION HSU_OS_BUS_SPACE_READ_1

DESCRIPTION
  Reads a BYTE from a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the data that was read.

SIDE EFFECTS
  None.
===========================================================================*/
/* Read a Byte from the resource */
juint8_t hsu_os_bus_space_read_1
(
  os_bus_resource_h res,
  juint32_t         offset
)
{
  memory_resource_info_type* 
    memory_resource_ptr = (memory_resource_info_type*)res;
  juint8_t ret_val;

  hsu_lpm_info_type* core_lpm_info;
  boolean is_in_lpm;

  HSU_USE_PARAM(core_lpm_info);
  HSU_USE_PARAM(is_in_lpm);

  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */

  core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);
  
  /* If for some reason we are in LPM, force exiting LPM */
  is_in_lpm = core_lpm_info->is_in_low_power_mode;

  if (is_in_lpm)
  {
    hsu_os_bus_core_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("hsu_os_bus_space_read_1: forced LPM exit");
  }


  HSU_ASSERT(offset < memory_resource_ptr->length);

  ret_val = HAL_hsusb_Read8(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address+offset);

  if (is_in_lpm)
  {
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  return ret_val;

} /* hsu_os_bus_space_read_1 */


/*===========================================================================
FUNCTION HSU_OS_BUS_SPACE_READ_2

DESCRIPTION
  Reads a WORD (2 bytes) from a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the data that was read.

SIDE EFFECTS
  None.
===========================================================================*/
juint16_t hsu_os_bus_space_read_2
(
  os_bus_resource_h res,
  juint32_t         offset
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;
  juint16_t ret_val;

  hsu_lpm_info_type* core_lpm_info;
  boolean is_in_lpm;

  HSU_USE_PARAM(core_lpm_info);
  HSU_USE_PARAM(is_in_lpm);

  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 2. */
  HSU_ASSERT((offset & 0x1) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */

  core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);
  
  /* If for some reason we are in LPM, force exiting LPM */
  is_in_lpm = core_lpm_info->is_in_low_power_mode;

  if (is_in_lpm)
  {
    hsu_os_bus_core_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("hsu_os_bus_space_read_2: forced LPM exit");
  }


  HSU_ASSERT(offset < memory_resource_ptr->length);

  ret_val = HAL_hsusb_Read16(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset);

  if (is_in_lpm)
  {
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  return ret_val;
} /* hsu_os_bus_space_read_2 */


/*===========================================================================
FUNCTION HSU_OS_BUS_SPACE_READ_4

DESCRIPTION
  Reads a DWORD (4 bytes) from a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the data that was read.

SIDE EFFECTS
  None.
===========================================================================*/
juint32_t hsu_os_bus_space_read_4
(
  os_bus_resource_h res,
  juint32_t         offset
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;
  juint32_t ret_val;

  hsu_lpm_info_type* core_lpm_info;
  boolean is_in_lpm;

  HSU_USE_PARAM(core_lpm_info);
  HSU_USE_PARAM(is_in_lpm);


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 4. */
  HSU_ASSERT((offset & 0x3) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */

  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  is_in_lpm = core_lpm_info->is_in_low_power_mode;

  if (is_in_lpm)
  {
    hsu_os_bus_core_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("hsu_os_bus_space_read_4: forced LPM exit");
  }


  HSU_ASSERT(offset < memory_resource_ptr->length);

  ret_val = HAL_hsusb_Read32(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset);

  if (is_in_lpm)
  {
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  return ret_val;
} /* hsu_os_bus_space_read_4 */


/*===========================================================================
FUNCTION HSU_OS_BUS_SPACE_WRITE_1

DESCRIPTION
  Writes a BYTE to a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_space_write_1
(
  os_bus_resource_h res,
  juint32_t         offset,
  juint8_t          value
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  hsu_lpm_info_type* core_lpm_info;
  boolean is_in_lpm;

  HSU_USE_PARAM(core_lpm_info);
  HSU_USE_PARAM(is_in_lpm);


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */

   core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  is_in_lpm = core_lpm_info->is_in_low_power_mode;

  if (is_in_lpm)
  {
    hsu_os_bus_core_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("hsu_os_bus_space_write_1: forced LPM exit");
  }

  HSU_ASSERT(offset < memory_resource_ptr->length);

  HAL_hsusb_Write8(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset, value);

  if (is_in_lpm)
  {
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

} /* hsu_os_bus_space_write_1 */


/*===========================================================================
FUNCTION HSU_OS_BUS_SPACE_WRITE_2

DESCRIPTION
  Writes a WORD (2 bytes) to a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_space_write_2
(
  os_bus_resource_h res,
  juint32_t         offset,
  juint16_t         value
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  hsu_lpm_info_type* core_lpm_info;
  boolean is_in_lpm;

  HSU_USE_PARAM(core_lpm_info);
  HSU_USE_PARAM(is_in_lpm);


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 2. */
  HSU_ASSERT((offset & 0x1) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  
  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  is_in_lpm = core_lpm_info->is_in_low_power_mode;

  if (is_in_lpm)
  {
    hsu_os_bus_core_exit_lpm(memory_resource_ptr->core_index);
    HSU_MSG_HIGH("hsu_os_bus_space_write_2: forced LPM exit");
  }
  

  HSU_ASSERT(offset < memory_resource_ptr->length);

  HAL_hsusb_Write16(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset, value);

  if (is_in_lpm)
  {
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);

    #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
    if (memory_resource_ptr->core_index == HS_USB_CORE_INDEX)
    {
      /* Schedule a USB cable status check. */
      hsu_timer_set(
        &(core_lpm_info->cable_status_check_timer),
        core_lpm_info->lpm_entry_timer_value_ms,
        T_MSEC);  
      
      core_lpm_info->enable_cable_status_check = TRUE;
    }
    #endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  }
  

} /* hsu_os_bus_space_write_2 */


/*===========================================================================
FUNCTION HSU_OS_BUS_SPACE_WRITE_4

DESCRIPTION
  Writes a DWORD to a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_space_write_4
(
  os_bus_resource_h res,
  juint32_t         offset,
  juint32_t         value
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  hsu_lpm_info_type* core_lpm_info;
  boolean is_in_lpm;

  HSU_USE_PARAM(core_lpm_info);
  HSU_USE_PARAM(is_in_lpm);


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 4. */
  HSU_ASSERT((offset & 0x3) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  
  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  is_in_lpm = core_lpm_info->is_in_low_power_mode;

  if (is_in_lpm)
  {
    hsu_os_bus_core_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("hsu_os_bus_space_write_4: forced LPM exit");
  }


  HSU_ASSERT(offset < memory_resource_ptr->length);

  HAL_hsusb_Write32(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset, value);

  if (is_in_lpm)
  {
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

} /* hsu_os_bus_space_write_4 */


/*===========================================================================
FUNCTION HSU_OS_BUS_START_STOP_LPM_TIMER

DESCRIPTION
  This function starts/stops the LPM timer and cancels any pending cable status 
  check depending on the received parameter (TRUE - start LPM timer, FALSE - Stop)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_start_stop_lpm_timer(uint32 core_index, boolean start_timer)
{
  hsu_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  if (start_timer)
  {
    if (core_index == HS_USB_CORE_INDEX)
    {
      /* Cancel any pending cable status check. */
      hsu_timer_cancel(&(core_lpm_info->cable_status_check_timer));
      core_lpm_info->enable_cable_status_check = FALSE;
      HSU_MSG_HIGH("hsu_os_bus_start_stop_lpm_timer: Cable status check timer canceled.");
    }

    /* Arm the low power mode entry timer. */
    HSU_MSG_HIGH("hsu_os_bus_start_stop_lpm_timer: scheduling LPM entry");
    hsu_os_bus_schedule_lpm_entry(core_index);
  }
  else
  {
    /* Cancel any pending entry of the Phy into low-power mode. */
    hsu_os_bus_cancel_scheduled_lpm_entry(core_index);
    HSU_MSG_HIGH("hsu_os_bus_start_stop_lpm_timer: LPM entry was canceled.");

    if (core_index == HS_USB_CORE_INDEX)
    {
      /* Cancel any pending cable status check. */
      hsu_timer_cancel(&(core_lpm_info->cable_status_check_timer));
      core_lpm_info->enable_cable_status_check = FALSE;
      HSU_MSG_HIGH("hsu_os_bus_start_stop_lpm_timer: Cable status check timer canceled.");
    }
  }
}  /* hsu_os_bus_start_stop_lpm_timer */

/*===========================================================================
FUNCTION HSU_OS_BUS_GET_NUM_CORES

DESCRIPTION
  This function returns the number of cores supported.

DEPENDENCIES
  None.

RETURN VALUE
  Number of supported cores.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hsu_os_bus_get_num_cores(void)
{
  return NUMBER_OF_CORES;
}

#ifdef FEATURE_HS_USB_POLLING
/*===========================================================================

FUNCTION hsu_os_bus_init_polling

DESCRIPTION
  This function performs target dependant initialization for polling mode operation. 
  
DEPENDENCIES
  The function should run on polling mode. 

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/

void hsu_os_bus_init_polling(juint8_t core_id)
{
  /* enable enterrupt  */
  if (file_static_vars_ptr->is_in_low_power_mode)
  {
    int loop_count = 0;

    /* turn on USBH_P_CLK */
    HSU_MSM_SET(GLBL_CLK_ENA, AXI_LI_APPS_CLK_ENA);
    HSU_MSM_SET(GLBL_CLK_ENA, USBH_P_CLK_ENA);

    /* Disable asynchronous interrupts from the PHY. */
    HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 0);

    /* Let the STP signal to get out from the core to the PHY.
    */
    HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 0);

    /* Set the PHCD bit in the port control register to zero. 
    ** This restarts the PHY clock and transfers the PHY into
    ** normal operating state.
    */
    HWIO_OUTF(USB_OTG_HS_PORTSC, PHCD, 0);

    /* Wait until the PHY finished its transition out 
    ** of low power mode.
    */
    while (HWIO_INF(USB_OTG_HS_PORTSC, PHCD) != 0)
    {
      /* Busy-wait loop. */
      HSU_ASSERT(loop_count < HSU_MAX_PHCD_DWELL_TIME_ATTEMPT);
      loop_count++;
    }

    /* This flag MUST be updated as the last thing here, after the low-power
    ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
    ** hardware too early.
    */
    file_static_vars_ptr->is_in_low_power_mode = FALSE;
  }  /* file_static_vars_ptr->is_in_low_power_mode */
}  /* hsu_os_bus_init_polling */

#endif /* FEATURE_HS_USB_POLLING */

/*===========================================================================
FUNCTION HSU_OS_BUS_INIT_CORE_INDEX_MAPS

DESCRIPTION
  This function initializes the arrays used for translation between 
  hsu_os_abstract_bus index, and HAL (or core_config) core index.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_init_core_index_maps(void)
{
#ifdef FEATURE_HS_USB_SNPS_CORE
  if(hsu_conf_sel_is_usb_core_snps()) 
  {
    file_static_vars_ptr->hal_core_index_map_ptr[USB3_CORE_INDEX] = 
      HSU_CORE_CONFIG_USB3_IDX;
    file_static_vars_ptr->hal_core_index_inv_map_ptr[HSU_CORE_CONFIG_USB3_IDX] = 
      USB3_CORE_INDEX;
  }
#endif
}

/*===========================================================================
FUNCTION HSU_OS_BUS_MEMORY_BARRIER

DESCRIPTION
  Memory barrier performs a strongly ordered write to system memory which causes
  flushing of all buffered data to be actually written to the system memory.

  Memory barrier operation is necessary on ARM11 that uses buffered memory 
  that is shared between ARM11 and USB HW (LINK controller).

  Memory barrier is not needed on ARM9 since there is no buffered memory 
  region on ARM9.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_memory_barrier( void )
{
  #if defined(FEATURE_HS_USB_MEMORY_BARRIER)
  memory_barrier();
  #endif /* FEATURE_HS_USB_MEMORY_BARRIER */
}


#if defined(FEATURE_HSU_TEST_COMPONENT)
/*===========================================================================
FUNCTION HSU_OS_BUS_GET_CORE_DESCRIPTION

DESCRIPTION
  This function returns the core description (name).

DEPENDENCIES
  None.

RETURN VALUE
  pointer to description

SIDE EFFECTS
  None.
===========================================================================*/
const char* hsu_os_bus_get_core_description( uint32 core_index )
{
  const usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;

  return core_table_ptr[core_index].description;

}
#endif /* FEATURE_HSU_TEST_COMPONENT */

#if defined(FEATURE_HSU_TEST_COMPONENT)
/*===========================================================================
FUNCTION HSU_OS_BUS_GET_CORE_LPM_STATUS

DESCRIPTION
  This function returns the core LPM status.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE          - core in LPM
  FALSE         - core not in LPM

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_get_core_lpm_status( uint32 core_index )
{
  const usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;

  return core_table_ptr[core_index].usb_lpm_info.is_in_low_power_mode;
}
#endif /* FEATURE_HSU_TEST_COMPONENT */


/*===========================================================================
FUNCTION hsu_os_bus_hal_core_id

DESCRIPTION
  This function is to externalize the macro OS_BUS_TO_HAL_CORE_ID

DEPENDENCIES
  None.

RETURN VALUE
  Core ID for HAL.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 hsu_os_bus_hal_core_id(uint8 core_id)
{
  return OS_BUS_TO_HAL_CORE_ID(core_id);
}

/*===========================================================================
FUNCTION hsu_hal_os_bus_core_id

DESCRIPTION
This function is to externalize the macro OS_BUS_FROM_HAL_CORE_ID

DEPENDENCIES
None.

RETURN VALUE
Core ID for HAL.

SIDE EFFECTS
None.
===========================================================================*/
uint8 hsu_hal_os_bus_core_id(uint8 core_id)
{
  return OS_BUS_FROM_HAL_CORE_ID(core_id);
}

/*===========================================================================
FUNCTION HSU_OS_BUS_SET_LPM_FEATURE_IS_ACTIVE
       	 
DESCRIPTION
This function is called during config_selector_init() and sets the value of
lpm_feature_is_active according to the NV parameter hs_usb_disable_lpm.
 	 
DEPENDENCIES
None.
 	 
RETURN VALUE
None.
 
SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_set_lpm_feature_is_active(boolean enable_lpm)
{
  file_static_vars_ptr->lpm_feature_is_active = enable_lpm;
} /* hsu_os_bus_set_lpm_feature_is_active */

/*===========================================================================
FUNCTION HSU_OS_BUS_CORE_ENTER_LPM

DESCRIPTION
Enter a core into low-power mode state.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_core_enter_lpm(uint32 core_index, 
                               boolean called_by_lpm_timer,
                               boolean is_analog_audio_mode)
{
  hsu_lpm_info_type* core_lpm_info;
  uint32 internal_core_index = OS_BUS_FROM_HAL_CORE_ID(core_index);

  core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[internal_core_index].usb_lpm_info);

  if (core_lpm_info->enter_lpm_handler)
  {
    core_lpm_info->enter_lpm_handler(core_index, called_by_lpm_timer, PWR_EVNT_SW);
  }  
}

/*===========================================================================
FUNCTION HSU_OS_BUS_CORE_EXIT_LPM

DESCRIPTION
Exit a core from a low-power mode state.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_core_exit_lpm(uint32 core_index) /* logical index */
{
  hsu_lpm_info_type* core_lpm_info;

  core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  if (core_lpm_info->exit_lpm_handler)
  {
    core_lpm_info->exit_lpm_handler(core_index, PWR_EVNT_SW);
  }  
}


/*===========================================================================
FUNCTION HSU_CORES_HW_INIT

DESCRIPTION
Wrapper function for initialization of all cores.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_cores_hw_init()
{
  uint32 core_index;
  usb_core_info_type* core_info_ptr;

  /* Hardware Initialization - Clock and LPM for each core */
  for (core_index = 0; core_index < NUMBER_OF_CORES ; core_index++)
  {
#ifndef FEATURE_HS_USB_NO_CI_CORE
#ifdef FEATURE_HS_USB_SNPS_CORE  
    if (hsu_conf_sel_is_usb_core_snps() 
        && core_index == HS_USB_CORE_INDEX)
    {
      /* For SNPS core is enabled, then skip CI USB core resource 
      ** initialization 
      */
      continue;
    } 
    else if((FALSE == hsu_conf_sel_is_usb_core_snps()) && (core_index == USB3_CORE_INDEX))
    {
      continue;
    }
#endif /* FEATURE_HS_USB_SNPS_CORE */
#endif // FEATURE_HS_USB_NO_CI_CORE

    core_info_ptr = &(file_static_vars_ptr->core_table_ptr[core_index]);
    core_info_ptr->core_hw_init(); 
  }
}

#ifndef FEATURE_HS_USB_NO_CI_CORE
/*===========================================================================
FUNCTION HSU_CORE_HW_INIT

DESCRIPTION
Hardware Initialization for OTG Core

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void otg_core_hw_init(void)
{
  hsu_vregmgr_init();
  hsu_vregmgr_turn_on(HSU_VREG_ID_HSUSB);

  /* initialize hsu clock manager for OTG core*/
  hsu_clkmgr_init();

  /* initialize hsu lpm for OTG core */
  hsu_lpm_init();
}
#endif // FEATURE_HS_USB_NO_CI_CORE

#ifdef FEATURE_HS_USB_SNPS_CORE
static void snps_core_hw_init(void)
{
  /* TODO : FEATURE_HS_USB_PRE_SILICON_BU 
  ** 1. Initialize and enable LDOs
  ** 2. Initialize clock manager.
  ** 3. Enable low power mode.
  */
  /* 1. Initialize LDO nodes with NPA */
  hsu_vregmgr_init();
  
  /* 2. Turn ON LDOs */
  hsu_vregmgr_turn_on(HSU_VREG_ID_HSUSB);

  /* 3. Initialize clock manager, and turn ON clocks for SNPS Core */
  hsu_clkmgr_init();

  /* 4. Initialize LPM manager for SNPS core */
  hsu_lpm_snps_init();
}
#endif /* FEATURE_HS_USB_SNPS_CORE */

/*===========================================================================
HSU_OS_ABSTRACT_BUS_GET_LPM_INFO

DESCRIPTION
Returns Low Power Mode information on the passed core index (logical)

DEPENDENCIES
None.

RETURN VALUE
Low Power Mode information

SIDE EFFECTS
None.
===========================================================================*/
hsu_lpm_info_type *hsu_os_abstract_bus_get_lpm_info
(
  uint32 core_index   /* logical core index*/
) 
{
  return &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
}


#ifdef FEATURE_HS_USB_F3

/*===========================================================================
FUNCTION HSU_OS_AP2MDM_STATUS_GPIO_CB

DESCRIPTION
  Callback funtion notifies BAM state machine that HOST is READY
  and unregisters GPIO 30 for interrupt.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
 void hsu_os_ap2mdm_status_gpio_cb(void* param)
{
  // TODO: --- Akash ---
  #ifdef CI_MERGE
  /* Interrupt processing code should be extremely short.
  ** Anything that takes a longer time should be processed by signalling
  ** the client task.
  */
  DALResult           dal_result; 
  hsu_platform_info_type *pinfo = (hsu_platform_info_type*)param;
  /* Deregister a GPIO interrupt handler.  This also disables the interrupt */
  dal_result = GPIOInt_DeregisterIsr(
    hsu_gpio_hsic_ready_ctx.h_gpio_int, (uint32)pinfo->gpio_hsic_ready,
    (GPIOINTISR)hsu_os_ap2mdm_status_gpio_cb);
  
  HSU_ASSERT(dal_result == DAL_SUCCESS);

  HSU_ULOG(NO_MSG, BUS_LOG, "hsu_call_hsic_start");

  hsu_bam_sm_send_sig_evt(HSU_BAM_SET_SIG_EVT(HSU_AP_SIDEBAND_SIG, HSU_HOST_READY_RISE_EVT));

  #endif
  return;  
}


 /*===========================================================================
FUNCTION HSU_OS_AP2MDM_STATUS_GPIO_REGISTER

DESCRIPTION
  Initializes GPIO 30 on MDM and registers the call back function.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_ap2mdm_status_gpio_register(void* param)
{
  // TODO: --- Akash ---
#ifdef CI_MERGE

  /* Declare a handle to the TLMM object */
  DALResult           dal_result; 
  hsu_platform_info_type const *platform_info;
  HSU_ASSERT(param);
  platform_info = (hsu_platform_info_type const*)param;

  /* Attach to a DAL */
  dal_result = DAL_DeviceAttach(
    DALDEVICEID_TLMM, 
    &hsu_gpio_hsic_ready_ctx.h_tlmm);
  
  HSU_ASSERT((dal_result == DAL_SUCCESS) && hsu_gpio_hsic_ready_ctx.h_tlmm);
 
  dal_result = DAL_DeviceAttach(
    DALDEVICEID_GPIOINT,
    &hsu_gpio_hsic_ready_ctx.h_gpio_int);
  
  HSU_ASSERT((dal_result == DAL_SUCCESS) && hsu_gpio_hsic_ready_ctx.h_gpio_int);


  /* Create a GPIO configuration  */
  hsu_gpio_hsic_ready_ctx.pin_config = DAL_GPIO_CFG(
    platform_info->gpio_hsic_ready,
    0,
    DAL_GPIO_INPUT,
    DAL_GPIO_NO_PULL,
    DAL_GPIO_2MA); 

  /* Call the TLMM DAL API to configure the GPIO */
  dal_result = DalTlmm_ConfigGpio(
    hsu_gpio_hsic_ready_ctx.h_tlmm, 
    hsu_gpio_hsic_ready_ctx.pin_config,
    DAL_TLMM_GPIO_ENABLE);
  
  HSU_ASSERT(dal_result == DAL_SUCCESS);

   /* Register the client callback with the GPIO Interrupt Controller */
  dal_result = GPIOInt_RegisterIsr(
    hsu_gpio_hsic_ready_ctx.h_gpio_int, 
    platform_info->gpio_hsic_ready,
    GPIOINT_TRIGGER_HIGH,
    (GPIOINTISR)hsu_os_ap2mdm_status_gpio_cb, 
    (uint32)platform_info);

  HSU_ASSERT(dal_result == DAL_SUCCESS);

  HSU_ULOG_FUNC(BUS_LOG);
#endif /*CI_MERGE*/
  return;
}

#ifdef FEATURE_HS_USB_VDDMIN_LOG
/*===========================================================================
FUNCTION HSU_OS_VDDMIN_GPIO_CB

DESCRIPTION
  Callback for GPIO 27 that logs VDDMIN events
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
 void hsu_os_vddmin_gpio_cb(void* param)
{
  DALResult dal_result = 0; 
  hsu_platform_info_type const*pinfo = (hsu_platform_info_type const*)param;

  if(vdd_min_gpio_high)
  {
    vdd_min_gpio_high = FALSE;
    dal_result = GPIOInt_SetTrigger(hsu_apq2mdm_gpio_intr_ctx.h_gpio_int, (uint32)
      pinfo->gpio_apq2mdm_vdd_min, GPIOINT_TRIGGER_LOW);
    HSU_ASSERT(dal_result == DAL_SUCCESS);
    /* GPIO is high means we are out of VDD min */
    hsu_vddmin_log(HSU_VDDMIN_EXIT_LOG, 0);
  }
  else
  {      
    vdd_min_gpio_high = TRUE;
    dal_result = GPIOInt_SetTrigger(hsu_apq2mdm_gpio_intr_ctx.h_gpio_int, (uint32)
      pinfo->gpio_apq2mdm_vdd_min, GPIOINT_TRIGGER_HIGH);
    HSU_ASSERT(dal_result == DAL_SUCCESS);
    /* GPIO is low means we are in VDD min */
    hsu_vddmin_log(HSU_VDDMIN_ENTER_LOG, 0);
  }

  return;  
}
#endif

/*===========================================================================
FUNCTION HSU_OS_VDDMIN_GPIO_REGISTER

DESCRIPTION
  Initializes GPIO for notification from APQ about VDDMIN events.
  Registers GPIO on MDM for VDDMIN events to be sent to APQ.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_vddmin_gpio_register(void* param)
{
#ifdef FEATURE_HS_USB_VDDMIN_LOG
  DALResult           dal_result = 0;   
  hsu_platform_info_type const *platform_info = (hsu_platform_info_type const*)param;

  //Init the kvp request object.
  kvp_t *rpm_request = kvp_create(0);
  uint32 gpio_num = platform_info->gpio_mdm2apq_vdd_min;
  uint32 gpio_mode = GPIO_TOGGLE_VDD_MIN;
  unsigned last_message_id;

  // Configure RPM to toggle GPIO to peer chip when it goes to VDD min
  kvp_put(rpm_request, REQ_GPIO_NUM, sizeof(uint32), &gpio_num);
  kvp_put(rpm_request, REQ_GPIO_MODE, sizeof(uint32), &gpio_mode);

  //Send the request to RPM
  last_message_id = rpm_post_request(RPM_ACTIVE_SET, RPM_GPIO_TOGGLE_REQ, 0, rpm_request);
  HSU_ULOG_1(NO_MSG, DATA_LOG, "rpm_request 0x%08X", (uint32)rpm_request);

  //Now block until the request is synchronized and processed by the RPM.
  rpm_barrier(last_message_id);
  hsu_vddmin_log(HSU_VDDMIN_RPM_REGISTERED, last_message_id);
 
  // Setup recieving Vdd min GPIO toggle from the peer chip

  vdd_min_gpio_high = FALSE;

  // Attach to a DAL
  dal_result = DAL_DeviceAttach(
    DALDEVICEID_TLMM, 
    &hsu_apq2mdm_gpio_intr_ctx.h_tlmm);
  
  HSU_ASSERT((dal_result == DAL_SUCCESS) && hsu_apq2mdm_gpio_intr_ctx.h_tlmm);

  dal_result = DAL_DeviceAttach(
    DALDEVICEID_GPIOINT,
    &hsu_apq2mdm_gpio_intr_ctx.h_gpio_int);

  HSU_ASSERT((dal_result == DAL_SUCCESS) && hsu_apq2mdm_gpio_intr_ctx.h_gpio_int);

  // Create a GPIO configuration
  hsu_apq2mdm_gpio_intr_ctx.pin_config= DAL_GPIO_CFG(
    platform_info->gpio_apq2mdm_vdd_min,
    0,
    DAL_GPIO_INPUT,
    DAL_GPIO_NO_PULL,
    DAL_GPIO_2MA); 

  // Call the TLMM DAL API to configure the GPIO
  dal_result = DalTlmm_ConfigGpio(
    hsu_apq2mdm_gpio_intr_ctx.h_tlmm, 
    hsu_apq2mdm_gpio_intr_ctx.pin_config,
    DAL_TLMM_GPIO_ENABLE);
  
  HSU_ASSERT(dal_result == DAL_SUCCESS);

  // Register the client callback with the GPIO Interrupt Controller
  dal_result = GPIOInt_RegisterIsr(
    hsu_apq2mdm_gpio_intr_ctx.h_gpio_int, 
    platform_info->gpio_apq2mdm_vdd_min,
    GPIOINT_TRIGGER_LOW,
    (GPIOINTISR)hsu_os_vddmin_gpio_cb, 
    (uint32)platform_info);
    
  HSU_ASSERT(dal_result == DAL_SUCCESS);
  
  // Mark the interrupt as non-wake for APQ
  mpmint_disable_gpio_wakeup(platform_info->gpio_apq2mdm_vdd_min);
#endif /* FEATURE_HS_USB_VDDMIN_LOG */

  return;
}
#ifdef FEATURE_HS_USB_VDDMIN_LOG
/*===========================================================================
FUNCTION HSU_OS_ABSTRACT_BUS_IS_APQ_IN_VDDMIN_STATE

DESCRIPTION
  Reads the APQ2MDM VDDmin GPIO to determine if APQ is in VDDmin.
  
DEPENDENCIES
  APQ2MDM Vddmin GPIO should be functional as expected. HIGH = not in VDDmin & LOW otherwise

RETURN VALUE
  TRUE if APQ is in VDDmin state FALSE otherwise

SIDE EFFECTS
  Based on the return value the HSIC decides weather to use GPIO or USB bus remote wake up.
===========================================================================*/
boolean hsu_os_abstract_bus_is_apq_in_vddmin_state(void)
{
  
  DALResult dRes;
  DALGpioValueType gpio_val;
  
  dRes = DalTlmm_GpioIn(hsu_apq2mdm_gpio_intr_ctx.h_tlmm, 
    hsu_apq2mdm_gpio_intr_ctx.pin_config, &gpio_val);
  if (dRes != DAL_SUCCESS)
  {
      HSU_ERR_FATAL("HSUSB could not get GPIO value", 0, 0, 0);
  }

  return (gpio_val == DAL_GPIO_LOW_VALUE) ? TRUE: FALSE;
}
#endif /* FEATURE_HS_USB_VDDMIN_LOG */
#endif /* FEATURE_HS_USB_F3 */

/*===========================================================================
FUNCTION HSU_OS_BUS_SEND_EVENT_TO_CORE

DESCRIPTION
  This function sets events to event variable in interrupt resources 
  and calls the USB interrupt handler.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_send_event_to_core
(
  uint32                      core_index, 
  hsu_os_bus_ext_isr_evt_type event
)
{
  interrupt_resource_info_type **isr_list;
  uint32                        isr_list_num;
  int                           iter;

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  INTLOCK_SAVE(hsu_os_intlock_flag)
  /* broadcast the event on interrupt resources */
  isr_list = file_static_vars_ptr->core_table_ptr[core_index].isr_list;

  isr_list_num = file_static_vars_ptr->
                   core_table_ptr[core_index].num_of_registered_isrs;
  
  for(iter=0; iter< isr_list_num; iter++)
  {
    isr_list[iter]->event |= event;
  }

  /* Call ISR WRAPPER to trigger the event. */
  hsu_os_hs_isr(core_index, PWR_EVNT_SW);
  INTFREE_RESTORE(hsu_os_intlock_flag)
}


/*===========================================================================
FUNCTION HSU_OS_BUS_SAVE_MEM_DUMP

DESCRIPTION
  This function saves the following items in a predefined static memory:

    -USB device controller registers
    -USB host controller registers
    -ULPI registers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
void hsu_os_bus_save_mem_dump(void)
{
#ifdef FEATURE_HS_USB_DUMP_USB_PHY_REG
  uint8 phy_register;
#endif /* FEATURE_HS_USB_DUMP_USB_PHY_REG */

  uint32 i;
  hsu_core_type_enum core_type;
  uint32 reg_read;

/*
 * uint32 static hsu_dump_end_tick ;
 * uint32 static hsu_dump_begin_tick ;
 */
  /* Pointer to the the memory resource of the primary core*/
  bus_resource_type* const bus_resource = 
    &(file_static_vars_ptr->core_table_ptr[HSU_CORE_IDX_PRIMARY].controllers_list[0].resource_list[1]);

  memory_resource_info_type* const mem_res_ptr = 
    &(bus_resource->resource_instance.mem_resource_instance);

  INTLOCK_SAVE(hsu_os_intlock_flag)

  /* hsu_dump_begin_tick = timetick_get(); */

  core_type = HSU_UKNOWN_CORE;

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  if (HSU_CORE_IDX_PRIMARY == HSU_CORE_CONFIG_USB3_IDX)
  {
    core_type = HSU_USB3_SNPS_CORE;
  }
#endif /* !HSU_CORE_CONFIG_HS_MAIN_IDX */

  /* check for usb perpheral clock is on */ 
  if ((core_type != HSU_UKNOWN_CORE) && 
       hsu_clkmgr_is_peripheral_clock_on(core_type))
  {
    /* Save OTG controller registers: */
    for (i = 0; i < OTG_REG_MEM_SIZE/4; i++)
    {
      *(hsu_os_abstract_bus.otg_controller_mem_dump_ptr + i) =  
        HAL_hsusb_Read32(OS_BUS_TO_HAL_CORE_ID(mem_res_ptr->core_index),
                          (uint32)(mem_res_ptr->start_address + i*4));
    }
#ifdef FEATURE_HS_USB_BAM
    /* BAM core dump APIs */
    hsu_al_bam_utils_core_dump();
#endif
#ifdef FEATURE_HS_USB_DUMP_USB_PHY_REG
    /* PHY dump requires all related clocks to be enabled */
    if (hsu_clkmgr_is_clock_on(core_type))
    {
      /* Read the ULPI registers */
      for (phy_register = 0; phy_register < ULPI_REG_MEM_SIZE; phy_register++)
      {
        (void)hsu_os_bus_read_from_phy_no_reset(phy_register,
              &hsu_os_abstract_bus.ulpi_reg_mem_dump_ptr[phy_register],HSU_CORE_IDX_PRIMARY);
      }
    }
#endif /* FEATURE_HS_USB_DUMP_USB_PHY_REG */

  }

  /* hsu_dump_end_tick = timetick_get(); */

  INTFREE_RESTORE(hsu_os_intlock_flag)
}
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */


