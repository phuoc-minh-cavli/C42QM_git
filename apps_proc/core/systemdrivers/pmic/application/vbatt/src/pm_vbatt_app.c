/*! \file
*  
*  \brief  pm_vbatt_app.c
*  \details Implementation file for vbatt information.
*    
*  &copy; Copyright 2020 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/17   kcs     Updated the file for more Battery management API's (CR -2110414).
05/29/17   kcs     created.


===========================================================================*/

/*===========================================================================

                    INCLUDE FILES 

===========================================================================*/
#include "pmapp_vbatt.h"
#include "CoreVerify.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "DALDeviceId.h"
#include "I2cTransferCfg.h"
#include "I2cTransfer.h"
#include "ddii2c.h"
#include "pm_utils.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "qurt_txm_qapi_fwk.h"
#include "qurt_txm_uid.h"

#define BATTERY_MINIMUM_VOLTAGE 		2300
#define BATTERY_MAXIMUM_VOLTAGE 		4800    

#define SMB_INTERRUPT_STATUS_REGISTER_A 0x38
#define SMB_INTERRUPT_STATUS_REGISTER_B 0x39
#define SMB_CHARGING_STATUS_REGISTER_B  0x3D
#define SMB_CHARGING_STATUS_REGISTER_C  0x3E
#define SMB_CHARGING_STATUS_REGISTER_A  0X3C
#define SMB_CHARGING_SYSOK_OPTIONS      0X07

#define DEBUGBOARD_BAT_ID_LL                    6375        //debug board bat id margin is 15% -/+ of 7.5K  
#define DEBUGBOARD_BAT_ID_UL                    8625 
/*===========================================================================

                        GLOBAL VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                        GLOBAL VARIABLES DEFINITIONS

===========================================================================*/
extern  boolean	bPMICStub;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

static pm_err_flag_type pmapp_get_battery_presence(void);                          /*Battery presence check */ 
static DALResult pmapp_vbatt_configure_i2c_once(void);				               /* I2C Configuration */
static DALResult pmapp_vbatt_configure_smb_once(void); 				               /* SMB Configuration */
static DALResult pmapp_vbatt_i2c_open(void);							           /* I2C Handle Open */
static DALResult pmapp_vbatt_i2c_close(void);							           /* I2C Handle Close */
static DALResult pmapp_vbatt_i2c_write(uint8 offset, uint8 data);		           /* I2C Write */
static DALResult pmapp_vbatt_i2c_read(uint8 offset, uint8 *data);		           /* I2C Read */

/* Flag to check if Battery presence */                            
static boolean batt_present = TRUE;  // Assume by default battery present

/*===========================================================================

                   Static Variable Definitions

===========================================================================*/

static boolean g_adc_init = FALSE;
static boolean g_vbatt_init  = FALSE;
static boolean g_smb_charger = SMB_PRESENT; /* Assume Default is SMB Platform */

static DalDeviceHandle *g_pmapp_i2c_handle;
static I2cTransfer      g_pmapp_i2c_xfer_write;
static I2cTransfer      g_pmapp_i2c_xfer_read;
static I2cClientConfig  g_pmapp_i2c_client_config;

uint32 g_vbattdbg_i2c_open_fail_count;  /* For Debugging - to track I2C Open Fail in DSR  */
uint32 g_vbattdbg_i2c_close_fail_count;	/* For Debugging - to track I2C close Fail in DSR */
uint32 g_vbattdbg_i2c_read_fail_count;  /* For Debugging - to track I2C Read fails 		  */
uint32 g_vbattdbg_i2c_write_fail_count;	/* For Debugging - to track I2C Write fails		  */

pmapp_battery_data battery_stats = {0, 0,FALSE,BAT_GOOD, PMIC_BAT_TECH_LI_ION, BAT_DISCHARGING, BAT_GOOD_HEALTH, CHG_ABSENT};

/*===========================================================================

                   FUNCTION IMPLEMENTATION 

===========================================================================*/

static DALResult pmapp_vbatt_i2c_open()
{
  DALResult result = DAL_SUCCESS;

  result = DalDevice_Open(g_pmapp_i2c_handle, DAL_OPEN_SHARED); 	
  if(result != DAL_SUCCESS)
  {
    g_vbattdbg_i2c_open_fail_count++;
  }
  return result;
}

static DALResult pmapp_vbatt_i2c_close()
{
  DALResult result = DAL_SUCCESS;

  result = DalDevice_Close(g_pmapp_i2c_handle);
  if(result != DAL_SUCCESS)
  {
    g_vbattdbg_i2c_close_fail_count++;
  }
  return result;	
}

DALResult pmapp_vbatt_configure_i2c_once()
{
  DALResult result = DAL_SUCCESS;
  
  g_pmapp_i2c_client_config.uByteTransferTimeoutUs  = 2500;
  g_pmapp_i2c_client_config.uBusFreqKhz			    = 100;  

  g_pmapp_i2c_xfer_write.tranCfg.uSlaveAddr         = 0x12;
  g_pmapp_i2c_xfer_write.eTranDirection	 	        = I2cTranDirOut;
  g_pmapp_i2c_xfer_write.eTranCtxt		   	        = I2cTrCtxNotASequence;

  g_pmapp_i2c_xfer_read.tranCfg.uSlaveAddr	        = 0x12;
  g_pmapp_i2c_xfer_read.eTranDirection		        = I2cTranDirIn;		
  g_pmapp_i2c_xfer_read.eTranCtxt			        = I2cTrCtxNotASequence;   

  /*Attach*/
  result = DAL_I2CDeviceAttach(DALDEVICEID_I2C_DEVICE_2, &g_pmapp_i2c_handle); 
  if (DAL_SUCCESS == result)
  {
    result = pmapp_vbatt_i2c_open();
    if(result != DAL_SUCCESS)
    {
      return  PM_ERR_FLAG__I2C_OPT2_ERR;
    }
    
  } 
  return result;
}

DALResult pmapp_vbatt_i2c_write(uint8 offset, uint8 data)
{
  DALResult     result           			   = DAL_SUCCESS;
  uint32        u32NumCompleted  			   = 0;
  unsigned char i2c_write_buf[2]               = {0,0}; 
  I2cBuffDesc   clientWriteBuff; 

  i2c_write_buf[0]         	                   = offset;
  i2c_write_buf[1]                             = data;
  clientWriteBuff.pBuff	  	                   = i2c_write_buf;    // Write offset and Data
  clientWriteBuff.uBuffSize 	               = 2;
  g_pmapp_i2c_xfer_write.pI2cBuffDesc          = &clientWriteBuff;
  g_pmapp_i2c_xfer_write.uTrSize   	           = 2;

  result = DalI2C_Write(g_pmapp_i2c_handle, &g_pmapp_i2c_xfer_write, &g_pmapp_i2c_client_config, &u32NumCompleted);
  if(result != DAL_SUCCESS)
  {
    g_vbattdbg_i2c_write_fail_count++;
    return  PM_ERR_FLAG__I2C_OPT2_ERR;
  }

  return result;
}


DALResult pmapp_vbatt_i2c_read(uint8 offset, uint8 *pData)
{	
  DALResult result                 = DAL_SUCCESS;
  uint32 u32NumCompleted     	   = 0;
  I2cBuffDesc clientReadBuff; 
  I2cBuffDesc clientWriteBuff; 

  unsigned char i2c_read_buf[2]        = {0,0};
  unsigned char i2c_write_buf[2]       = {0,0}; 

  i2c_write_buf[0]         	 	       = offset;
  clientWriteBuff.pBuff	  	  	       = i2c_write_buf;    // Write offset 
  clientWriteBuff.uBuffSize 	       = 1;
  g_pmapp_i2c_xfer_write.pI2cBuffDesc  = &clientWriteBuff;
  g_pmapp_i2c_xfer_write.uTrSize   	   = 1;

  result = DalI2C_Write(g_pmapp_i2c_handle, &g_pmapp_i2c_xfer_write, &g_pmapp_i2c_client_config, &u32NumCompleted);
  if(result != DAL_SUCCESS)
  {
    g_vbattdbg_i2c_write_fail_count++;
    return  PM_ERR_FLAG__I2C_OPT2_ERR;
  }

  u32NumCompleted           	           = 0; // reset num read
  i2c_read_buf[0]                          = 0; // reset buffer - only 1 byte read
  clientReadBuff.pBuff	                   = i2c_read_buf;
  clientReadBuff.uBuffSize                 = 1; 
  g_pmapp_i2c_xfer_read.pI2cBuffDesc       = &clientReadBuff;
  g_pmapp_i2c_xfer_read.uTrSize	           = 1;

  result = DalI2C_Read(g_pmapp_i2c_handle, &g_pmapp_i2c_xfer_read, &g_pmapp_i2c_client_config, &u32NumCompleted);
  if(result != DAL_SUCCESS)
  {
    g_vbattdbg_i2c_read_fail_count++;
    return  PM_ERR_FLAG__I2C_OPT2_ERR;
  }

  *pData = i2c_read_buf[0];
	
	return result;
}

DALResult pmapp_vbatt_configure_smb_once()
{
  uint8 reg_val = 0;
  DALResult result = DAL_SUCCESS;
  uint32 battery_id = 0;
  

  pmapp_vbatt_i2c_read(0x30, &reg_val);
  reg_val = (reg_val & (~0x80)) | 0x80;
  result  = pmapp_vbatt_i2c_write(0x30, reg_val);  // Allow volatile writes and don't set  Charging bit high as Charging enable polarity is Active Low

  result |= pm_utils_get_battery_id(&battery_id);
  if (result != PM_ERR_FLAG__SUCCESS)
  {
    return result;
  }
  if(battery_id >= DEBUGBOARD_BAT_ID_LL && battery_id <= DEBUGBOARD_BAT_ID_UL)
  {
    pmapp_vbatt_i2c_read(0x30, &reg_val);
    reg_val = (reg_val & (~0x02)) | 0x02;
    result  = pmapp_vbatt_i2c_write(0x30, reg_val);  //  disable the charging as debug board detected 
  }

  reg_val = 0;
  pmapp_vbatt_i2c_read(0x06, &reg_val);
  reg_val = (reg_val & 0xFC) | 0x03;
  result  |= pmapp_vbatt_i2c_write(0x06, reg_val);  // Enables battery Missing Detection via THERM IO
  
    reg_val = 0;
  pmapp_vbatt_i2c_read(0x09, &reg_val);
  reg_val = (reg_val & (~0x20)) | 0x20;                
  result  |= pmapp_vbatt_i2c_write(0x09, reg_val);  // Enables battery Missing detection Config enable

  return result;
}

pm_err_flag_type pmapp_get_battery_presence()
{
    uint8             read_data = 0x0;
    DALResult         result    = DAL_SUCCESS;
    pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
    
    result = pmapp_vbatt_i2c_open();
    if(DAL_SUCCESS != result) 
    {
      return PM_ERR_FLAG__I2C_OPT_ERR;
    }
    result = pmapp_vbatt_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_B, &read_data);
    if( DAL_SUCCESS != result)
    {
      return PM_ERR_FLAG__I2C_OPT_ERR;
    }
    /* Gives battery presence */
    if(read_data & 0x10)
    {
        batt_present = FALSE;  // battery absent
    }
    else
    {
       batt_present = TRUE; // battery present
    }

    result = pmapp_vbatt_i2c_close();
    if(result != DAL_SUCCESS)
    {
      return  PM_ERR_FLAG__I2C_OPT2_ERR;
    }
    
    return err_flag;
}


pm_err_flag_type pmapp_get_battery_stats_init()
{
  pm_err_flag_type                 err_flag 	        = PM_ERR_FLAG__SUCCESS;
  DalDeviceHandle				  *phPlatformInfoDevice = NULL;
  DalChipInfoFamilyType 		   eChipInfoFamilyType	= DALCHIPINFO_FAMILY_UNKNOWN;
  DalPlatformInfoPlatformInfoType  platformInfo 		= {DALPLATFORMINFO_TYPE_UNKNOWN,};
  uint8 reg_val = 0;

  boolean						   status	= FALSE;
  DALResult 					   result	= DAL_SUCCESS;
	
  if (bPMICStub == TRUE)
  {
    return err_flag;
  }

  if(g_vbatt_init == FALSE)
  {
          
        //Initialize the SMB, if it returns success ,we can assume it as a smb target
        // Configure I2C 
        result = pmapp_vbatt_configure_i2c_once();
        if(result != DAL_SUCCESS)
        {
            g_smb_charger = FALSE;
            return PM_ERR_FLAG__I2C_OPT_ERR; 
        }
		result = pmapp_vbatt_i2c_read(0x00, &reg_val);  //dummy read, to know whether slave exists or not. 
		if(result != DAL_SUCCESS)
		{
           g_smb_charger = FALSE;
        }
        else
        {
          g_smb_charger = TRUE; 
        }

      if (g_smb_charger)
      {
        // Configure SMB
        result = pmapp_vbatt_configure_smb_once();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        // check for batterry presence
        err_flag = pmapp_get_battery_presence();
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
    
        // Close I2C as it'll reopened when needed
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
      }
    
    g_vbatt_init = TRUE;
    
  }
    
return err_flag;
}

pm_err_flag_type pmapp_vbatt_get_battery_status(uint8* battery_percentage)
{
	uint32 current_voltage = 0x0;
	pm_err_flag_type       err_flag 	 = PM_ERR_FLAG__SUCCESS;
    uint8 pmic_index = 0;
    uint8             read_data = 0x0;
    DALResult  result	= DAL_SUCCESS;
    TXM_MODULE_INSTANCE *module_instance = NULL;
	
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }
	
    if(battery_percentage == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }

    else
    {   
      err_flag = pm_utils_get_battery_volt_level(&current_voltage);
      if (PMIC_IS_PME9205 == pm_get_pmic_model(pmic_index))  // adding this WA due the limitation(don't have Vbat adc channel) of yoda 
      {
        result = pmapp_vbatt_i2c_open();
        if(DAL_SUCCESS != result) 
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_read(SMB_CHARGING_STATUS_REGISTER_A, &read_data);
        if( DAL_SUCCESS != result)
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        if(read_data & 0x4)
        {
            current_voltage = (current_voltage) - (((current_voltage-2500)/50)+265); 
        }
      }
      if(err_flag != PM_ERR_FLAG__SUCCESS)
      {
      	return err_flag;
      }
      if(current_voltage < BATTERY_MINIMUM_VOLTAGE) 
      {
         current_voltage = BATTERY_MINIMUM_VOLTAGE; 
      }
      battery_stats.bat_voltage = current_voltage;
     // Percentage = ((current_voltage - min_voltage) / (max_voltage - min_voltage)) * 100
      battery_stats.bat_percentage = ((current_voltage - BATTERY_MINIMUM_VOLTAGE) * 100 )/ (BATTERY_MAXIMUM_VOLTAGE - BATTERY_MINIMUM_VOLTAGE ); 
      if(battery_stats.bat_percentage > 100)
      {
      		battery_stats.bat_percentage = 100;
      }
      if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
      {
        return PM_ERR_FLAG__INVALID_POINTER;
      }   
      //*battery_percentage = battery_stats.bat_percentage;
      if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)battery_percentage, (qurt_size_t)sizeof(uint8),
              (qurt_addr_t *)(&battery_stats.bat_percentage), (qurt_size_t)sizeof(battery_stats.bat_percentage)))
      {
          return PM_ERR_FLAG__INVALID_POINTER;
      }
    }

	return err_flag;
}


pm_err_flag_type pmapp_get_battery_health(pm_battery_health *batt_health)
{
    DALResult         result        = DAL_SUCCESS;
    pm_err_flag_type  err_flag	    = PM_ERR_FLAG__SUCCESS;
    uint8             read_data     = 0x0;
	TXM_MODULE_INSTANCE *module_instance = NULL;
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }
        
    if(batt_health == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }
    
    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }

    else
    {
        result = pmapp_vbatt_i2c_open();
        if(DAL_SUCCESS != result) 
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
    
        result = pmapp_vbatt_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_B, &read_data);
        if( DAL_SUCCESS != result)
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        /* Better during I2C init Write the Float Voltage 0x3 register to MAX_VOLTAGE 4200 ??? */
        /* Max Voltage depends on battery */
        /* Gives battery presence */
        switch (read_data & 0x54)
        {
          case 0x40:
            /* Batery is Over-Voltage means 100mV above Float Voltage */
            /* Float Voltage means the max the battery can be charged */
            battery_stats.bat_health = BAT_OV;
          break;

          case 0x04:
            /* Batery is Low-Battery-Voltage < 2.8V*/
            battery_stats.bat_health = BAT_UV;
          break;

          case 0x10:
            /* Batery is Missing*/
            battery_stats.bat_health = BAT_MISSING;
          break;

        
          default:
            /* Batery Voltage is Good*/
            battery_stats.bat_health = BAT_GOOD_HEALTH;
          break;
        }
        if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
        {
          return PM_ERR_FLAG__INVALID_POINTER;
        }
        if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)batt_health, (qurt_size_t)sizeof(pm_battery_health),
                (qurt_addr_t *)(&battery_stats.bat_health), (qurt_size_t)sizeof(battery_stats.bat_health)))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

    }
    
    return err_flag;
}

pm_err_flag_type pmapp_get_battery_temp(pm_battery_temperature *batt_temp)
{
    DALResult         result    = DAL_SUCCESS;
    pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
    uint8             read_data = 0x0;   
    TXM_MODULE_INSTANCE *module_instance = NULL;
	
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }
    if(batt_temp == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }
    
    else
    {
        result = pmapp_vbatt_i2c_open();
        if(DAL_SUCCESS != result) 
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }

        result = pmapp_vbatt_i2c_read(SMB_INTERRUPT_STATUS_REGISTER_A, &read_data);
        if( DAL_SUCCESS != result)
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        switch (read_data & 0x55)
        {
          case 0x11:
          case 0x10:
          case 0x01:
            /* Batery is Cold <0deg C */
            battery_stats.bat_temp = BAT_COLD;
          break;

          case 0x44:
          case 0x40:
          case 0x04:
            /* Batery is Hot > 40deg C*/
            battery_stats.bat_temp = BAT_HOT;
          break;

          default:
            /* No BAT_THERM connected and hence Batery is Good */
            battery_stats.bat_temp = BAT_GOOD;
          break;

        }
        if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }		
        if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)batt_temp, (qurt_size_t)sizeof(pm_battery_temperature),
                (qurt_addr_t *)(&battery_stats.bat_temp), (qurt_size_t)sizeof(battery_stats.bat_temp)))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

    }
    
    return err_flag;
}

pm_err_flag_type pmapp_get_battery_tech(pm_battery_technology *batt_tech)
{
    pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;    
    TXM_MODULE_INSTANCE *module_instance = NULL;
    
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }
   
    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }

    if(batt_tech == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    
    else
    {
         /* No battery profile and hence reporting as LI-ION with out reading any register */
        battery_stats.bat_technology = PMIC_BAT_TECH_LI_ION;
        if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }		
        if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)batt_tech, (qurt_size_t)sizeof(pm_battery_technology),
                (qurt_addr_t *)(&battery_stats.bat_technology), (qurt_size_t)sizeof(battery_stats.bat_technology)))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }
    }
    
    return err_flag;
}

pm_err_flag_type pmapp_get_charging_status(pm_battery_chg_status *charge_status)
{
    DALResult         result    = DAL_SUCCESS;
    pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
    uint8             read_data = 0x0;   
    TXM_MODULE_INSTANCE *module_instance = NULL;
	
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }

    if(charge_status == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }
  
    else
    {
        result = pmapp_vbatt_i2c_open();
        if(DAL_SUCCESS != result) 
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_read(SMB_CHARGING_STATUS_REGISTER_B, &read_data);
        if( DAL_SUCCESS != result)
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        switch (read_data & 0x06)
        {
          case 0x0:
            /* Not Charging */
            battery_stats.bat_chg_status = BAT_DISCHARGING;
          break;

          case 0x2:
            /* Pre Charging - Below Low Bat Threshold - Charges with Pre-charge current*/
          case 0x4:
            /* Fast Charging - When battery Voltage b/w Pre and Float - charges with constant Charge current FCC*/
          case 0x6:
            /* Taper Charging - When battery reaches Float Voltage - Charger starts reducing the charge current*/
          default:
            battery_stats.bat_chg_status = BAT_CHARGING;
          break;
        }
        if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }		
        if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)charge_status, (qurt_size_t)sizeof(pm_battery_chg_status),
                (qurt_addr_t *)(&battery_stats.bat_chg_status), (qurt_size_t)sizeof(battery_stats.bat_chg_status)))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }
    }
	return err_flag;
}

pm_err_flag_type pmapp_get_charger_source(pm_battery_chg_src *charger_source)
{
    DALResult         result    = DAL_SUCCESS;
    pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
    uint8             read_data = 0x0;   
    DalChipInfoFamilyType 		   eChipInfoFamilyType	= DALCHIPINFO_FAMILY_UNKNOWN;
    TXM_MODULE_INSTANCE *module_instance = NULL;
	
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }

    if(charger_source == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }
    
    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }
    
    else
    {
        //There is no USB data line connection between USB connector and SMB231 for MDM9205
        eChipInfoFamilyType = DalChipInfo_ChipFamily();
        if (eChipInfoFamilyType == DALCHIPINFO_FAMILY_MDM9205)
        {
		  return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
		}
        result = pmapp_vbatt_i2c_open();
        if(DAL_SUCCESS != result) 
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_read(SMB_CHARGING_STATUS_REGISTER_C, &read_data);
        if( DAL_SUCCESS != result)
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        if(battery_stats.vbus_present)
        {
          switch (read_data & 0x0F)
          {
            case 0x1:
              /* Charging Downstream Port*/
              /* PC CDP Port - Charges @ 1500mA */
              battery_stats.bat_chg_src = USB_CDP;
            break;

            case 0x2:
              /* Wall Adapter */
            case 0x3:
              /* Other Charging Port*/
              battery_stats.bat_chg_src = USB_DCP;
            break;

            case 0x0:
              /* Non Confirming Charger */
            case 0x4:
              /* Standard Downstream Port*/
              /* Normal USB Port and charges @ 500mA or 900mA or based USB2.0 or 3.0 respectively */
            default:
              /* For all other Charging ports like Docking station etc. report SDP*/
              battery_stats.bat_chg_src = USB_SDP;
            break;
          }
        }
        else
        {
          battery_stats.bat_chg_src = CHG_ABSENT;
        }
        if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }		
        if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)charger_source, (qurt_size_t)sizeof(pm_battery_chg_src),
                (qurt_addr_t *)(&battery_stats.bat_chg_src), (qurt_size_t)sizeof(battery_stats.bat_chg_src)))
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

    }
    return err_flag;
}

#ifdef FEATURE_PMIC_CHARGER
pm_err_flag_type pmapp_get_charge_reg(uint8 offset, uint8 *data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  DALResult ret = DAL_SUCCESS;
  TXM_MODULE_INSTANCE *module_instance = NULL;

  if (g_vbatt_init != TRUE)
  {
    pmapp_get_battery_stats_init();
  }

  if (data == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  ret = pmapp_vbatt_i2c_open();
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;

  ret = pmapp_vbatt_i2c_read(offset, data);
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;

  ret = pmapp_vbatt_i2c_close();
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT2_ERR;

  return err_flag;
}

pm_err_flag_type pmapp_set_charge_reg(uint8 offset, uint8 data)
{
  pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
  DALResult         ret       = DAL_SUCCESS;

  if (g_vbatt_init != TRUE)
  {
    pmapp_get_battery_stats_init();
  }

  ret = pmapp_vbatt_i2c_open();
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;

  ret = pmapp_vbatt_i2c_write(offset, data);
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;

  ret = pmapp_vbatt_i2c_close();
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT2_ERR;

  return err_flag;
}

pm_err_flag_type pmapp_vbatt_get_battery_volt_status(uint32* battery_volt)
{
	uint32 current_voltage = 0x0;
	pm_err_flag_type       err_flag 	 = PM_ERR_FLAG__SUCCESS;
    uint8 pmic_index = 0;
    uint8             read_data = 0x0;
    DALResult  result	= DAL_SUCCESS;
    TXM_MODULE_INSTANCE *module_instance = NULL;
	
    if(g_vbatt_init != TRUE)
    {
        pmapp_get_battery_stats_init();
    }
	
    if(battery_volt == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    if(batt_present == FALSE)
    {
        return PM_ERR_FLAG__BATTERY_MISSING;
    }

    else
    {   
      err_flag = pm_utils_get_battery_volt_level(&current_voltage);
      if (PMIC_IS_PME9205 == pm_get_pmic_model(pmic_index))  // adding this WA due the limitation(don't have Vbat adc channel) of yoda 
      {
        result = pmapp_vbatt_i2c_open();
        if(DAL_SUCCESS != result) 
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_read(SMB_CHARGING_STATUS_REGISTER_A, &read_data);
        if( DAL_SUCCESS != result)
        {
          return PM_ERR_FLAG__I2C_OPT_ERR;
        }
        result = pmapp_vbatt_i2c_close();
        if(result != DAL_SUCCESS)
        {
          return  PM_ERR_FLAG__I2C_OPT2_ERR;
        }
        if(read_data & 0x4)
        {
            current_voltage = (current_voltage) - (((current_voltage-2500)/50)+265); 
        }
      }
      if(err_flag != PM_ERR_FLAG__SUCCESS)
      {
      	return err_flag;
      }
      if(current_voltage < BATTERY_MINIMUM_VOLTAGE) 
      {
         current_voltage = BATTERY_MINIMUM_VOLTAGE; 
      }
      battery_stats.bat_voltage = current_voltage;
      if ( QURT_EOK != qurt_txm_module_instance_get(&module_instance))
      {
        return PM_ERR_FLAG__INVALID_POINTER;
      }   
      
      if( QURT_EOK != qurt_copy_to_user_ext((TXM_MODULE_INSTANCE *)module_instance,(qurt_addr_t *)battery_volt, (qurt_size_t)sizeof(uint32),
              (qurt_addr_t *)(&battery_stats.bat_voltage), (qurt_size_t)sizeof(battery_stats.bat_voltage)))
      {
          return PM_ERR_FLAG__INVALID_POINTER;
      }
    }

	return err_flag;
}


pm_err_flag_type pmapp_config_sysok_option(uint8 option)
{
  pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
  DALResult         ret       = DAL_SUCCESS;
  uint8 read_data;

  if(option > 3)
	  return PM_ERR_FLAG__INPUT_INVALID;
  
  if (g_vbatt_init != TRUE)
  {
    pmapp_get_battery_stats_init();
  }

  ret = pmapp_vbatt_i2c_open();
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;
  
  ret = pmapp_vbatt_i2c_read(SMB_CHARGING_SYSOK_OPTIONS, &read_data);
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;
  
  read_data = (read_data & (~0x03)) | option;
  
  ret = pmapp_vbatt_i2c_write(SMB_CHARGING_SYSOK_OPTIONS, read_data);
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT_ERR;

  ret = pmapp_vbatt_i2c_close();
  if (DAL_SUCCESS != ret) return PM_ERR_FLAG__I2C_OPT2_ERR;

  return err_flag;
}
#endif
