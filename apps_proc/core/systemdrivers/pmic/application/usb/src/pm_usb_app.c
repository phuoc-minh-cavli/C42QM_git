/*! \file pm_usb_app.c
*  \n
*  \brief USB Application APIs implementation.
*  \n  
*  \n &copy; Copyright 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/application/usb/src/pm_usb_app.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/17    kcs     Updated the VBUS status info in the global structure (CR -2110414).
10/21/16    kc      Added I2c Open and Close APIs in DSR to allow VDD Min (CR-1080814) 
10/05/16    kc      Add support for VBUS detection through MPP2/Non-SMB (CR-1079022)
09/15/16    kc      Add support for VBUS detection through MPP2/SMB (CR-1067741) 
10/14/13    rh      Created file
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "DALSys.h"
#include "DDISpmi.h"
#include "pmapp_usb.h"
#include "pm_err_flags.h"
#include "pm_version.h"
#include "pm_malloc.h"
#include "pm_resources_and_types.h"
#include "pm_vbus_driver.h"
#include "pm_ulog.h"
#include "DALDeviceId.h"
#include "I2cTransferCfg.h"
#include "I2cTransfer.h"
#include "ddii2c.h"
#include "dsr.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "pmapp_vbatt.h"
#include "pm_utils.h"
#include "qmi_client.h"
#include "specific_absorption_rate_v01.h"
#include "qurt_mutex.h"

#define SMB_INTERRUPT_STATUS_REGISTER_A 		0x38
#define SMB_INTERRUPT_STATUS_REGISTER_B 		0x39
#define SMB_INTERRUPT_STATUS_REGISTER_C  		0x3A
#define SMB_INTERRUPT_STATUS_REGISTER_D  		0x3B
#define SMB_CHARGING_STATUS_REGISTER_A          0X3C

#define MAX_DATA_SIZE  							2
#define DEBUGBOARD_BAT_ID_LL                    6375        //debug board bat id margin is 15% -/+ of 7.5K  
#define DEBUGBOARD_BAT_ID_UL                    8625 

#define QMI_CLIENT_INSTANCE_SAR 0

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static DALResult pmapp_usb_configure_i2c_once(void);				/* I2C Configuration */
static DALResult pmapp_usb_configure_smb_once(void); 				/* SMB Configuration */
static pm_err_flag_type pmapp_usb_configure_mpp_once(void); 		/* MPP Configuration */
static void* pmapp_usb_spmi_isr(void* me, uint32 intrMask); 		/* SPMI ISR */
static void pmapp_usb_dsr_callback(dsr_obj_t*, void *, void *);		/* DSR Callback*/
static DALResult pmapp_usb_i2c_open(void);							/* I2C Handle Open */
static DALResult pmapp_usb_i2c_close(void);							/* I2C Handle Close */
static DALResult pmapp_usb_i2c_write(uint8 offset, uint8 data);		/* I2C Write */
static DALResult pmapp_usb_i2c_read(uint8 offset, uint8 *data);		/* I2C Read */
pm_err_flag_type pmapp_usb_qmi_register(void);                      /* QMI registration part to pass Vbus info to modem  */
static qurt_mutex_t clnt_mtx;                                       /* Protect cb and write-read request collisions  */
void pmapp_qmi_msg_async_cb                                         /* qmi cb for async message seding   */
(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                    *buf,
  unsigned int            len,
  void                    *resp_cb_data,
  qmi_client_error_type   transp_err
);
static sar_set_usb_conn_state_req_msg_v01 vbus_state_req;
static sar_set_usb_conn_state_resp_msg_v01 vbus_state_resp;


/*===========================================================================

                        GLOBAL VARIABLES DEFINITIONS

===========================================================================*/


extern boolean  bPMICStub;

static DalDeviceHandle *g_i2c_handle;
static I2cTransfer      g_transfer_write;
static I2cTransfer      g_transfer_read;
static I2cClientConfig  g_client_config;
static dsr_obj_t       *g_usb_dsr_obj;
static boolean          g_smb_opt = TRUE; /* Assume Default is SMB Platform */

uint32 g_debug_dsr_enque_fail;     		/* For Debugging - to track DSR Enqueue Fail      */
uint32 g_debug_spmi_isr_count;	   		/* For Debugging - to track ISR trigger Count     */
uint32 g_debug_dsr_callback_count;		/* For Debugging - to track DSR Execute Count 
										   and is always ISR trigger count + 1            */
uint32 g_debug_i2c_open_fail_count;		/* For Debugging - to track I2C Open Fail in DSR  */
uint32 g_debug_i2c_close_fail_count;	/* For Debugging - to track I2C close Fail in DSR */
uint32 g_debug_i2c_read_fail_count;		/* For Debugging - to track I2C Read fails 		  */
uint32 g_debug_i2c_write_fail_count;	/* For Debugging - to track I2C Write fails		  */

DALResult g_debug_i2c_write_fail_return_value;	/* For Debugging - to track I2C Write fails */
DALResult g_debug_i2c_read_fail_return_value;	/* For Debugging - to track I2C Write fails		  */


static qmi_client_type         vbus_client;
static boolean is_qmi_cient_up = FALSE;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pmapp_usb_vbus_status_callback(pmapp_usb_vbus_det_fn_cb pmapp_usb_cb)
{
    static pmapp_usb_isr_data        isr_data;
    static uint8                     run_once;
    static DalDeviceHandle          *dalIntHandle;
    uint8 reg_val = 0;
    uint8 pmic_index = 0;
	
	DalDeviceHandle                 *phPlatformInfoDevice = NULL;
	DalChipInfoFamilyType            eChipInfoFamilyType  = DALCHIPINFO_FAMILY_UNKNOWN;
	DalPlatformInfoPlatformInfoType  platformInfo         = {DALPLATFORMINFO_TYPE_UNKNOWN,};

	boolean                          status   = FALSE;
    DALResult                        result   = DAL_SUCCESS;
    pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
    
    pm_device_info_type pmic_device_info;

    if (bPMICStub == TRUE)
    {
        return err_flag;
    }
	
	err_flag = pm_get_pmic_info(0, &pmic_device_info);

    if(err_flag != PM_ERR_FLAG__SUCCESS)
    {
        return err_flag;
    }   

	if(run_once == 0)
    {
        //Initialize the SMB, if it returns success ,we can assume it as a smb target
        // Configure I2C 
        result = pmapp_usb_configure_i2c_once();
        if(result != DAL_SUCCESS)
        {
            g_smb_opt = FALSE;
            return PM_ERR_FLAG__I2C_OPT_ERR; 
        }
		result = pmapp_usb_i2c_read(0x00, &reg_val);  //dummy read, to know whether slave exists or not. 
		if(result != DAL_SUCCESS)
		{
           g_smb_opt = FALSE;
        }
        else
        {
          g_smb_opt = TRUE; 
        }

        if (g_smb_opt)
        {
            // Configure SMB
            result = pmapp_usb_configure_smb_once();
            if(result != DAL_SUCCESS)
            {
                return  PM_ERR_FLAG__I2C_OPT2_ERR;
            }
         
			if(result != DAL_SUCCESS)
			{
				return  PM_ERR_FLAG__I2C_OPT2_ERR;
			}
        }
		
		// Close I2C as it'll reopened when needed as part of VBUS handler
		result = pmapp_usb_i2c_close();
        // Configure MPP2
        err_flag = pmapp_usb_configure_mpp_once();
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
            return err_flag;
        }
		
		// Create deferred service routine for USB VBUS IRQ processing
		status = dsr_create(&g_usb_dsr_obj, (dsr_hdlr_t)pmapp_usb_dsr_callback, NULL, DSR_PRI_HIGH, DSR_CONTEXT_ISR);
		if(status != TRUE)
        {
			return PM_ERR_FLAG__DSR_SERVICE_FAIL; 
        }
        
        //Get DAL handle
        result = DAL_StringDeviceAttach("DALDEVICEID_SPMI_DEVICE", &dalIntHandle);
      
        if(result == DAL_SUCCESS)
        {
            result = DalDevice_Open(dalIntHandle, DAL_OPEN_SHARED);
        }
        
        if(result == DAL_SUCCESS)
        {
            isr_data.pmapp_usb_cb = pmapp_usb_cb;
            isr_data.pmic_device_info = pmic_device_info;
                 
            //Configure ISR for VBUS_USB
            result = DalSpmi_RegisterIsr(dalIntHandle, 0xC0, 0xFF, pmapp_usb_spmi_isr, &isr_data);
        }
        
        if(result != DAL_SUCCESS)
        {
            err_flag = PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED;
        }
        
        //For Initial USB status
        pmapp_usb_dsr_callback(g_usb_dsr_obj, NULL, &isr_data);
        if (PMIC_IS_PME9205 == pm_get_pmic_model(pmic_index))
        {
          err_flag |= pmapp_usb_qmi_register();
        }
        run_once = 1;
    }
	
	return err_flag;
}

DALResult pmapp_usb_configure_i2c_once()
{
    DALResult result = DAL_SUCCESS;


    g_client_config.uByteTransferTimeoutUs    = 2500;
	g_client_config.uBusFreqKhz			      = 100;  

	g_transfer_write.tranCfg.uSlaveAddr   = 0x12;
	g_transfer_write.eTranDirection	 	  = I2cTranDirOut;
	g_transfer_write.eTranCtxt		   	  = I2cTrCtxNotASequence;

	g_transfer_read.tranCfg.uSlaveAddr	  = 0x12;
	g_transfer_read.eTranDirection		  = I2cTranDirIn;		
	g_transfer_read.eTranCtxt			  = I2cTrCtxNotASequence;   

    /*Attach*/
    result = DAL_I2CDeviceAttach(DALDEVICEID_I2C_DEVICE_2, &g_i2c_handle); 
    if (DAL_SUCCESS == result)
    {
        result = pmapp_usb_i2c_open();
		if(result != DAL_SUCCESS)
		{
			return  PM_ERR_FLAG__I2C_OPT2_ERR;
		}
	} 
	return result;
}

static DALResult pmapp_usb_i2c_open()
{
	DALResult result = DAL_SUCCESS;

	result = DalDevice_Open(g_i2c_handle, DAL_OPEN_SHARED); 	
	if(result != DAL_SUCCESS)
	{
		g_debug_i2c_open_fail_count++;
	}
	return result;
}

static DALResult pmapp_usb_i2c_close()
{
	DALResult result = DAL_SUCCESS;
	
	result = DalDevice_Close(g_i2c_handle);
	if(result != DAL_SUCCESS)
	{
		g_debug_i2c_close_fail_count++;
	}
	return result;	
}


DALResult pmapp_usb_configure_smb_once()
{
    uint8 reg_val = 0;
    uint32 battery_id = 0;
    
    DALResult result = DAL_SUCCESS;

    pmapp_usb_i2c_read(0x30, &reg_val);
    reg_val = (reg_val & (~0x80)) | 0x80;
    result  = pmapp_usb_i2c_write(0x30, reg_val);  // Allow volatile writes and don't set Charging bit high as Charging enable polarity is Active Low
    
    result |= pm_utils_get_battery_id(&battery_id);
    if (result != PM_ERR_FLAG__SUCCESS)
    {
      return result;
    }
    if(battery_id >= DEBUGBOARD_BAT_ID_LL && battery_id <= DEBUGBOARD_BAT_ID_UL)
    {
      pmapp_usb_i2c_read(0x30, &reg_val);
      reg_val = (reg_val & (~0x02)) | 0x02;
      result  = pmapp_usb_i2c_write(0x30, reg_val);  //  disable the charging as debug board detected 
    }

    pmapp_usb_i2c_read(0x09, &reg_val);
    reg_val = (reg_val & (~0xFF) )| 0x01;
    result  = pmapp_usb_i2c_write(0x09, reg_val);  // disable IRQ except INOK IRQ

// made this below two smb writes to make VPH is algined with Vbat , as we are relaying on VPH for Vbat

    pmapp_usb_i2c_read(0x04, &reg_val);
    reg_val = (reg_val & (~0x03) )| 0x02;
    result  |= pmapp_usb_i2c_write(0x04, reg_val);  // system output voltages  Vbat+250mV    
    
    
    pmapp_usb_i2c_read(0x06, &reg_val);
    reg_val = (reg_val & (~0x30) )| 0x20;
    result  |= pmapp_usb_i2c_write(0x06, reg_val);  // System voltage threshold for initiating charge current reduction-- VBATT tracking (100 mV)    
    

    return result;
}

pm_err_flag_type pmapp_usb_configure_mpp_once() 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    pm_comm_write_byte(0x0,0xC011,0x01,0x0);     // Edge trigger interrupts
    pm_comm_write_byte(0x0,0xC012,0x01,0x0);     // Raising edge enable

    if (g_smb_opt)
    {
        pm_comm_write_byte(0x0,0xC042,0x05,0x0);     // no pull up enable

    }
    else
    {
        pm_comm_write_byte(0x0,0xC013,0x01,0x0);     // falling trigger enable
    }

    pm_comm_write_byte(0x0,0xC014,0x01,0x0);     // Clears the internal sticky
	pm_comm_write_byte(0x0,0xC015,0x01,0x0);     // Interrupt Enable
    pm_comm_write_byte(0x0,0xC040,0x00,0x0);     // DIGITAL_INPUT
    pm_comm_write_byte(0x0,0xC046,0x80,0x0);     // GPIO1_EN_CTL
    return err_flag;
}

DALResult pmapp_usb_i2c_write(uint8 offset, uint8 data)
{
	DALResult     result           			   = DAL_SUCCESS;
	uint32        u32NumCompleted  			   = 0;
	unsigned char i2c_write_buf[MAX_DATA_SIZE] = {0,0}; 
	I2cBuffDesc   clientWriteBuff; 

	i2c_write_buf[0]         	  = offset;
	i2c_write_buf[1]              = data;
	clientWriteBuff.pBuff	  	  = i2c_write_buf;    // Write offset and Data
	clientWriteBuff.uBuffSize 	  = 2;
	g_transfer_write.pI2cBuffDesc    = &clientWriteBuff;
	g_transfer_write.uTrSize   	  = 2;

	result = DalI2C_Write(g_i2c_handle, &g_transfer_write, &g_client_config, &u32NumCompleted);
	if(result != DAL_SUCCESS)
	{
		g_debug_i2c_write_fail_count++;
		g_debug_i2c_write_fail_return_value = result;
		return  result;
	}
	
	return result;
}

DALResult pmapp_usb_i2c_read(uint8 offset, uint8 *pData)
{	
	DALResult result               = DAL_SUCCESS;
	uint32 u32NumCompleted     	   = 0;
	I2cBuffDesc clientReadBuff; 
	I2cBuffDesc clientWriteBuff; 
	
	unsigned char i2c_read_buf[MAX_DATA_SIZE]  = {0,0};
	unsigned char i2c_write_buf[MAX_DATA_SIZE] = {0,0}; 

	i2c_write_buf[0]         	 			   = offset;
	clientWriteBuff.pBuff	  	  			   = i2c_write_buf;    // Write offset 
	clientWriteBuff.uBuffSize 	 			   = 1;
	g_transfer_write.pI2cBuffDesc    		   = &clientWriteBuff;
	g_transfer_write.uTrSize   	  			   = 1;
	
	result = DalI2C_Write(g_i2c_handle, &g_transfer_write, &g_client_config, &u32NumCompleted);
	if(result != DAL_SUCCESS)
	{
		g_debug_i2c_write_fail_count++;
        g_debug_i2c_write_fail_return_value = result;
		return  result;
	}
	
	u32NumCompleted           	 = 0; // reset num read
    i2c_read_buf[0]              = 0; // reset buffer - only 1 byte read
	clientReadBuff.pBuff	     = i2c_read_buf;
	clientReadBuff.uBuffSize     = 1; 
	g_transfer_read.pI2cBuffDesc = &clientReadBuff;
	g_transfer_read.uTrSize	     = 1;
	
    result = DalI2C_Read(g_i2c_handle, &g_transfer_read, &g_client_config, &u32NumCompleted);
	if(result != DAL_SUCCESS)
	{
		g_debug_i2c_read_fail_count++;
		g_debug_i2c_read_fail_return_value = result;
		return  result;
	}
	
	*pData = i2c_read_buf[0];
	
	return result;
}

static void* pmapp_usb_spmi_isr(void* me, uint32 intrMask)
{
	boolean             dsr_status = FALSE;
    pmapp_usb_isr_data* isr_data   = (pmapp_usb_isr_data*)me;
	
	dsr_status = dsr_enqueue(g_usb_dsr_obj, (void *)isr_data);
		
	if(TRUE !=dsr_status)
	{
		g_debug_dsr_enque_fail++;
	}
    
	g_debug_spmi_isr_count++;
	
	return NULL;
}

void pmapp_usb_dsr_callback(dsr_obj_t* dsr_obj, void *ctxt, void *payload)
{
    pmapp_usb_isr_data* isr_data      = (pmapp_usb_isr_data*)payload;
    uint8               irq_status[5] = {0};
    uint8               mpp_status    = 0;
    boolean             attach        = FALSE;
    boolean    is_qmi_cient_up_local  = FALSE;
    qmi_txn_handle txn_handle;
    int rc;    

    if(isr_data != NULL)
    {
        /* For SMB case, open I2C and check status */
        if (g_smb_opt)
        {
			 /* Open I2C handle */
			if(DAL_SUCCESS != pmapp_usb_i2c_open())	
			{
				return;
			}
		
            //Read SMB Status registers to clear the Interrupts
            pmapp_usb_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_A, &irq_status[0]);
            pmapp_usb_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_B, &irq_status[1]);
            pmapp_usb_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_C, &irq_status[2]);
            pmapp_usb_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_D, &irq_status[3]);
            pmapp_usb_i2c_read(SMB_CHARGING_STATUS_REGISTER_A, &irq_status[4]);
              
            
            /* close I2C handle */
			if(DAL_SUCCESS != pmapp_usb_i2c_close())	
			{
				return;
			}
     
            attach = ((irq_status[4] & 0x4) >>2);
			battery_stats.vbus_present = attach;
        }
        else
        {
            pm_comm_read_byte(0x0,0xC010,&mpp_status, 0x0); // MPP RT status
            attach = (mpp_status & 1);
			battery_stats.vbus_present = attach;
        }
            
        isr_data->pmapp_usb_cb(attach);

         // Clear MPP2 sticky bit 
		pm_comm_write_byte(0x0,0xC014,0x01,0x0); 
        
        qurt_mutex_lock(&clnt_mtx);
        is_qmi_cient_up_local = is_qmi_cient_up;
        qurt_mutex_unlock(&clnt_mtx);
        if(is_qmi_cient_up_local)
        {
            vbus_state_req.usb_is_connected_valid = (uint8_t)TRUE;
            vbus_state_req.usb_is_connected = (uint8_t) attach;
            rc = qmi_client_send_msg_async(vbus_client,
                                           QMI_SAR_SET_USB_CONN_STATE_REQ_V01,
                                           (void *)&vbus_state_req,
                                           sizeof(sar_set_usb_conn_state_req_msg_v01),
                                           (void *)&vbus_state_resp,
                                           sizeof(sar_set_usb_conn_state_resp_msg_v01),
                                           &pmapp_qmi_msg_async_cb,
                                           NULL,
                                           &txn_handle);
             if (rc != QMI_NO_ERR)
             {
                PM_LOG_MSG_INFO("qmi async call err %d",rc);
             }
        }
        
	}
    
	g_debug_dsr_callback_count++;
}


/*===========================================================================
  FUNCTION: pmapp_qmi_vbus_notify_cb_main
===========================================================================*/
/*!
    @brief
    Callback to receive MCS service events. 
    as part of this call back we upate the static variable is_qmi_cient_up once
    modem side sar serivce is up.

    @param
      user_handle - QCCI client ID
      service_obj - service object
      service_event - service event
      notify_cb_data - User data payload

    @return
    NONE
*/
/*=========================================================================*/
static void pmapp_qmi_vbus_notify_cb_main
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                          *notify_cb_data
)
{
  qmi_idl_service_object_type vbus_svc_obj;
  qmi_client_error_type qmi_err;
  qmi_service_info info;
  qmi_txn_handle txn_handle;
  int rc;
  boolean    is_qmi_cient_up_local  = FALSE;
  vbus_svc_obj = sar_get_service_object_v01();
  
   qurt_mutex_lock(&clnt_mtx);
   is_qmi_cient_up_local = is_qmi_cient_up; // don't create new client if we had already have one
   qurt_mutex_unlock(&clnt_mtx);
  
  if((QMI_CLIENT_SERVICE_COUNT_INC == service_event) && (FALSE == is_qmi_cient_up_local))
  {
    qmi_err = qmi_client_get_service_instance(vbus_svc_obj, QMI_CLIENT_INSTANCE_SAR, &info);
    if(qmi_err == QMI_NO_ERR)
    {
      qmi_err = qmi_client_init(&info, vbus_svc_obj, NULL, NULL, NULL, &vbus_client);
      if(qmi_err == QMI_NO_ERR)
      {
        qurt_mutex_lock(&clnt_mtx);
        is_qmi_cient_up = TRUE;
        qurt_mutex_unlock(&clnt_mtx);
        
        //For Initial Vbus status
        vbus_state_req.usb_is_connected_valid = (uint8_t)TRUE;
        vbus_state_req.usb_is_connected = (uint8_t) (battery_stats.vbus_present);
        rc = qmi_client_send_msg_async(vbus_client,
                                       QMI_SAR_SET_USB_CONN_STATE_REQ_V01,
                                       (void *)&vbus_state_req,
                                       sizeof(sar_set_usb_conn_state_req_msg_v01),
                                       (void *)&vbus_state_resp,
                                       sizeof(sar_set_usb_conn_state_resp_msg_v01),
                                       &pmapp_qmi_msg_async_cb,
                                       NULL,
                                       &txn_handle);
        if (rc != QMI_NO_ERR)
        {
           PM_LOG_MSG_INFO("qmi async call err %d",rc);
        }
                                   
      }
      else
      {
        PM_LOG_MSG_INFO("qmi client init err %d",qmi_err);
      }
    }
    else
    {
       PM_LOG_MSG_INFO("qmi service get instance err %d",qmi_err);
     }
  }
  else if((QMI_CLIENT_SERVICE_COUNT_DEC == service_event) && (TRUE == is_qmi_cient_up_local))
  {
    qmi_err = qmi_client_release(vbus_client);
    if(qmi_err == QMI_NO_ERR)
    {
      qurt_mutex_lock(&clnt_mtx);
      is_qmi_cient_up = FALSE;
      qurt_mutex_unlock(&clnt_mtx);
    }
    else
    {
      PM_LOG_MSG_INFO("qmi release err %d",qmi_err);
    }
  }
  
}

pm_err_flag_type pmapp_usb_qmi_register() 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  qmi_client_error_type qmi_err = QMI_NO_ERR;
  uint8 data =0 ;
  qmi_idl_service_object_type vbus_svc_obj;
  qmi_client_type vbus_notifier_main;

  vbus_svc_obj = sar_get_service_object_v01();
  if (NULL == vbus_svc_obj)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Register the DPM notification callback */
  qmi_err = qmi_client_notifier_init(vbus_svc_obj, NULL, &vbus_notifier_main);
  if (qmi_err != QMI_NO_ERR)
  {
    PM_LOG_MSG_INFO("qmi clt notifier err %d",battery_stats.vbus_present);
    return PM_ERR_FLAG__INVALID;
  }
  qurt_mutex_init(&clnt_mtx);
  qmi_err |= qmi_client_register_notify_cb(vbus_notifier_main, pmapp_qmi_vbus_notify_cb_main, &data);
  if (qmi_err != QMI_NO_ERR)
  {
    return PM_ERR_FLAG__INVALID;
  }
  return err_flag;
}


void pmapp_qmi_msg_async_cb
(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                    *buf,
  unsigned int            len,
  void                    *resp_cb_data,
  qmi_client_error_type   transp_err
)
{
   if(QMI_NO_ERR == transp_err)
   {
     PM_LOG_MSG_INFO("sent vbus %d",battery_stats.vbus_present);
   }
   else
   {
     PM_LOG_MSG_INFO("async cb err %d",transp_err);
   }
}

