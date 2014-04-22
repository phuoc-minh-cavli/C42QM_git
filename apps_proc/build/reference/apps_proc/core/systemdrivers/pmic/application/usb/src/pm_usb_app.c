/*! \file pm_usb_app.c
*  \n
*  \brief USB Application APIs implementation.
*  \n  
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tn/4.0/reference/apps_proc/core/systemdrivers/pmic/application/usb/src/pm_usb_app.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/16    kc		Added I2c Open and Close APIs in DSR to allow VDD Min (CR-1080814) 
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

#define SMB_INTERRUPT_STATUS_REGISTER_A 		0x35
#define SMB_INTERRUPT_STATUS_REGISTER_B 		0x36
#define SMB_INTERRUPT_STATUS_REGISTER_C  		0x37
#define SMB_INTERRUPT_STATUS_REGISTER_D  		0x38
#define SMB_INTERRUPT_STATUS_REGISTER_E 		0x39
#define SMB_INTERRUPT_STATUS_REGISTER_F 		0x3A

#define MAX_DATA_SIZE  							2

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



/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pmapp_usb_vbus_status_callback(pmapp_usb_vbus_det_fn_cb pmapp_usb_cb)
{
    static pmapp_usb_isr_data        isr_data;
    static uint8                     run_once;
    static DalDeviceHandle          *dalIntHandle;

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
	
		//Read platform id to check the method of VBUS detection - SMB or Non-SMB.
		eChipInfoFamilyType = DalChipInfo_ChipFamily();
		if (eChipInfoFamilyType == DALCHIPINFO_FAMILY_MDM9x07)
		{
			/* Get the platform info */
			result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
			if (result != DAL_SUCCESS)
			{
                return PM_ERR_FLAG__DAL_DEVICE_ATTACH_FAIL;
			}

			result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
			if (result != DAL_SUCCESS)
			{
                return PM_ERR_FLAG__DAL_PLATFORM_ERROR;
			}
			
			if((DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x02 == platformInfo.subtype) ||
			   (DALPLATFORMINFO_TYPE_CDP == platformInfo.platform && 0x01 == platformInfo.subtype) ||
			   (DALPLATFORMINFO_TYPE_RCM == platformInfo.platform && 0x01 == platformInfo.subtype) )
			{
				g_smb_opt = FALSE; // For Non-SMB platform
			}
		}
		
        if (g_smb_opt)
        {
            // Configure I2C 
            result = pmapp_usb_configure_i2c_once();
            if(result != DAL_SUCCESS)
            {
                return PM_ERR_FLAG__I2C_OPT_ERR; 
            }
            
            // Configure SMB
            result = pmapp_usb_configure_smb_once();
            if(result != DAL_SUCCESS)
            {
                return  PM_ERR_FLAG__I2C_OPT2_ERR;
            }
         
			// Close I2C as it'll reopened when needed as part of VBUS handler
			result = pmapp_usb_i2c_close();
			if(result != DAL_SUCCESS)
			{
				return  PM_ERR_FLAG__I2C_OPT2_ERR;
			}
        }
        
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
            result = DalSpmi_RegisterIsr(dalIntHandle, 0xA1, 0xFF, pmapp_usb_spmi_isr, &isr_data);
        }
        
        if(result != DAL_SUCCESS)
        {
            err_flag = PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED;
        }
		
		//For Initial USB status
		pmapp_usb_dsr_callback(g_usb_dsr_obj, NULL, &isr_data);
		
		run_once = 1;
        
    }
	
	return err_flag;
}

DALResult pmapp_usb_configure_i2c_once()
{
    DALResult result = DAL_SUCCESS;

    g_client_config.uByteTransferTimeoutUs    = 2500;
	g_client_config.uBusFreqKhz			      = 100;  

	g_transfer_write.tranCfg.uSlaveAddr   = 0x57;
	g_transfer_write.eTranDirection	 	  = I2cTranDirOut;
	g_transfer_write.eTranCtxt		   	  = I2cTrCtxNotASequence;

	g_transfer_read.tranCfg.uSlaveAddr	  = 0x57;
	g_transfer_read.eTranDirection		  = I2cTranDirIn;		
	g_transfer_read.eTranCtxt			  = I2cTrCtxNotASequence;   

    /*Attach*/
    result = DAL_I2CDeviceAttach(DALDEVICEID_I2C_DEVICE_4, &g_i2c_handle); 
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
    DALResult result = DAL_SUCCESS;
	
//	result  = pmapp_usb_i2c_write(0x30, 0x80);  // Enable Allow volatile writes to CONFIG registers
//	result += pmapp_usb_i2c_write(0x05, 0x00);  // 0x0 = STAT o/p - Active low, Indicates charging
//	result += pmapp_usb_i2c_write(0x06, 0x00);  // 0x0 = Disable LED blinking, charger IRQ and APSD IRQ
//	result += pmapp_usb_i2c_write(0x0C, 0x0C);  // 0xC = Enable USB OV,UV IRQs
	
    return result;
}

pm_err_flag_type pmapp_usb_configure_mpp_once()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    pm_comm_write_byte(0x0,0xA111,0x01,0x0);     // Edge trigger interrupts
	pm_comm_write_byte(0x0,0xA113,0x01,0x0);     // Falling edge enable

    if (g_smb_opt)
    {
		pm_comm_write_byte(0x0,0xA142,0x02,0x0); // PULLUP_SEL, 10K - Required for SMB option
    }
    else
    {
        pm_comm_write_byte(0x0,0xA112,0x01,0x0); // Rising edge enable, UP indicates Attach
		                                         // DOWN indicates Detach when MPP2 is directly connected to VBUS        
    }
    
    pm_comm_write_byte(0x0,0xA114,0x01,0x0);     // Clears the internal sticky
	pm_comm_write_byte(0x0,0xA115,0x01,0x0);     // Interrupt Enable
    pm_comm_write_byte(0x0,0xA140,0x00,0x0);     // DIGITAL_INPUT
    pm_comm_write_byte(0x0,0xA141,0x03,0x0);     // Select Voltage source, VIN3
    pm_comm_write_byte(0x0,0xA146,0x80,0x0);     // MPP2_EN_CTL
	
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
		return  PM_ERR_FLAG__I2C_OPT2_ERR;
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
		return  PM_ERR_FLAG__I2C_OPT2_ERR;
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
		return  PM_ERR_FLAG__I2C_OPT2_ERR;
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
    uint8               irq_status[6] = {0};
    uint8               mpp_status    = 0;
    boolean             attach        = FALSE;
		
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
            pmapp_usb_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_E, &irq_status[4]);
            pmapp_usb_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_F, &irq_status[5]);

            /* close I2C handle */
			if(DAL_SUCCESS != pmapp_usb_i2c_close())	
			{
				return;
			}
     
            attach = (irq_status[5] & 0x1);
        }
        else
        {
            pm_comm_read_byte(0x0,0xA110,&mpp_status, 0x0); // MPP RT status
            
            attach = (mpp_status & 1);
        }
            
        isr_data->pmapp_usb_cb(attach);

         // Clear MPP2 sticky bit 
		pm_comm_write_byte(0x0,0xA114,0x01,0x0); 
	}
	
	g_debug_dsr_callback_count++;
}


