/*
  @file qsee_api_tests.c
  @brief Contains test code for most of the QSEE APIs.

*/
/*===========================================================================
  Copyright (c) 2011-2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cdefs.h"
#include "qsee_env.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_sfs.h"
#include "qsee_fs.h"
#include "qsee_i2c.h"
#include "qsee_comstr.h"
#include "qsee_spi.h"
#include "qsee_tlmm.h"
#include "qsee_comstr.h"
#include "qsee_prng.h"
#include "qsee_core.h"
#include "qsee_dcache.h"
#include "qsee_services.h"
#include "qsee_sync.h"
#include "qsee_timer.h"
#include "qsee_trans_ns_addr.h"
#include "qsee_oem_buffer.h"
#include "qsee_intmask.h"
#include "qsee_securechannel.h"
#include "CDebugPolicySource.h"
#include "ISource.h"
#include "INSSystemReg.h"
#include "CNSSystemReg.h"
#include "IRTICReport.h"
#include "CRTICReport.h"
#include "qsee_hash.h"
#include "sampleapp.h"
#include "CSPI.h"
#include "ISPI.h"

#define TOUCH_I2C_ADDRESS 0x4a                /* Address of the touch controller on the I2C bus */
#define TOUCH_I2C_DEVICE  QSEE_I2CPD_DEVICE_1 /* Bus the touch controller is on, BLSP 1 */
//config for i2c
qsee_i2c_bus_config_t i2c_bus_config =
{
  100,                              // uFsBusFreqInKhz;
  0,                                // uHsBusFreqInKhz; not used
  QSEE_I2C_STANDARD_INTERFACE,     // eInterface;  not used
  2500                              // nMaxBusAcquireWaitTimeMsec;
};

qsee_i2c_slave_device_config_t i2c_slave_device_config =
{
  0x0,                             // uSlaveAddress;
  QSEE_I2C_7_BIT_SLAVE_ADDRESS,    // eSlaveAddressOption;
  QSEE_I2C_MEMORY_ADDRESS_DEVICE,  // eSlaveDeviceType;
  QSEE_I2C_GEN_START_BEFORE_READ,  // eReadOption;
  2500                              // nByteTransferWaitTimeUSec;
};

#define SPI_WRITE_BUF_SIZE 32*1024
#define SPI_READ_BUF_SIZE  32*1024
uint8_t *spi_read_data = NULL;
uint8_t *spi_write_data = NULL;

#define UUID_SIZE 16

/* Registers in the Atmel mXT Memory Map, Figure 2-1 of Protocol Guide */
#define REG_FAMILY_ID  0x00

/* MIN OEM IMEM size, being the minimum TZBSP_OEM_IMEM_SIZE defined across chipsets.
 * Effective size is read at runtime. */
#define TZBSP_OEM_IMEM_MIN_SIZE 0x800

#define ut_assert_eqi(xx, yy) if ((xx) != (yy)) {goto bail;}

#define TEST_ASSERT(xx, yy, str_fmt, ...)                                  \
   if ((xx) != (yy))                                                       \
   {qsee_log(QSEE_LOG_MSG_ERROR, "Failed! (0x%x != 0x%x) "                 \
             #str_fmt, (xx), (yy), ##__VA_ARGS__); return FAILED; } else { \
    qsee_log(QSEE_LOG_MSG_DEBUG, "Success! (0x%x == 0x%x) "                \
               #str_fmt, (xx), (yy), ##__VA_ARGS__);}

// allocate buffer size for debug policy content (max 4k for 8998)
#define TEST_DEBUGPOLICY_BUF_SIZE (0x1000)
// 8996 debug policy file size is 0x3C0. 8998 one will add new fields and size
// will be increased. This macro defines the minimum file size.
#define TEST_DEBUGPOLICY_FILE_SIZE (0x3C0)

static int run_security_state_test(void);

int run_core_test(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen, void* data, uint32_t len)
{
  uint32_t retval = 0;
  uint32_t output_buffer = 0;
  size_t uuid_size1 = UUID_SIZE;
  size_t uuid_size2 = UUID_SIZE;
  char uuid_buf1[UUID_SIZE] = {"aaaaaaaaaaaaaaaa"};
  char uuid_buf2[UUID_SIZE] = {"bbbbbbbbbbbbbbbb"};

  qsee_log(QSEE_LOG_MSG_ERROR, "-- CORE UNIT TESTS ---------------------------------");

  retval = qsee_prng_getdata((void*)(&output_buffer), sizeof(uint32_t));
  TEST_ASSERT(retval, sizeof(uint32_t), "qsee_prng_getdata(), output: %x", output_buffer);

  TEST_ASSERT(qsee_is_ns_range(data, len), true, "qsee_is_ns_range(data, len)");

#ifndef OFFTARGET
  TEST_ASSERT(qsee_is_ns_range(cmd, cmdlen), false, "qsee_is_ns_range(cmd, cmdlen)");
#endif

  qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_read_jtag_id() output: %x",qsee_read_jtag_id());
  qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_read_serial_num() output: %x",qsee_read_serial_num());

  qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_get_secure_stat() tests DONE (pass==1): %x",(0==run_security_state_test()));

  /* Get and print the device UUID */
  retval = qsee_get_device_uuid((uint8_t *) uuid_buf1, &uuid_size1);
  TEST_ASSERT(SUCCESS, retval, "qsee_get_device_uuid pass 1");

  TEST_ASSERT(uuid_size1, UUID_SIZE, "qsee_get_device_uuid pass 1 size. uuid_size: %u", uuid_size1);

  retval = qsee_get_device_uuid((uint8_t *) uuid_buf2, &uuid_size2);
  TEST_ASSERT(SUCCESS, retval, "qsee_get_device_uuid pass 2");

  TEST_ASSERT(uuid_size2, UUID_SIZE, "qsee_get_device_uuid pass 2 size. uuid_size: %u", uuid_size2);

  retval = memcmp(uuid_buf1, uuid_buf2, UUID_SIZE);
  TEST_ASSERT(0, retval, "1:%X%X%X%X-%X%X-%X%X-%X%X-%X%X%X%X%X%X 2:%X%X%X%X-%X%X-%X%X-%X%X-%X%X%X%X%X%X",
             uuid_buf1[0], uuid_buf1[1], uuid_buf1[2], uuid_buf1[3],
             uuid_buf1[4], uuid_buf1[5], uuid_buf1[6], uuid_buf1[7],
             uuid_buf1[8], uuid_buf1[9], uuid_buf1[10], uuid_buf1[11],
             uuid_buf1[12], uuid_buf1[13], uuid_buf1[14], uuid_buf1[15],
             uuid_buf2[0], uuid_buf2[1], uuid_buf2[2], uuid_buf2[3],
             uuid_buf2[4], uuid_buf2[5], uuid_buf2[6], uuid_buf2[7],
             uuid_buf2[8], uuid_buf2[9], uuid_buf2[10], uuid_buf2[11],
             uuid_buf2[12], uuid_buf2[13], uuid_buf2[14], uuid_buf2[15]);
  return 0;
}


int run_misc_test()
{
   WCHAR* w_a = (WCHAR*)qsee_malloc(128);
   char * a = (char*)qsee_malloc(64);
   char * b = (char*)qsee_malloc(64);

   qsee_log(QSEE_LOG_MSG_DEBUG, "-- COMSTR TESTS begin---------------------------------");

   if(!a || !b || !w_a)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_malloc() failed");
      return -1;
   }

   if(!qsee_strlcpy(a, "Test string", 32))  //Returns the number of bytes copied, if succssful.  Should be non-zero.
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strlcpy() failed");
      return -1;
   }

   if(!qsee_strlcpy(b, "Test string", 32))  //Returns the number of bytes copied, if succssful.  Should be non-zero.
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strlcpy() failed");
      return -1;
   }

   if(!qsee_strlcat(a, "-ending-", 32)) // (a's string length is now 19 chars, and then a NULL char at the end.)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strlcat() failed");
      return -1;
   }

   if(0 != qsee_strnicmp(a, b, 4)) //Compare only first 4 bytes.  Return should be 0.
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strnicmp() failed");
      return -1;
   }

   if(0 == qsee_strnicmp(a, b, 32)) //Compare all 32 bytes.  Return should NOT be 0.
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strnicmp() failed");
      return -1;
   }

   qsee_strtowstr(a, w_a, 64);

   if(19 != qsee_wstrlen(w_a))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strtowstr() or qsee_wstrlen() failed");
      return -1;
   }

   if(0 != qsee_wstrcmp(w_a, w_a))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrcmp() failed");
      return -1;
   }
#if 0
   if(NULL != qsee_wstrcmp(NULL, NULL))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrcmp() failed, NULL was acceptes as a ptr to a string");
      return -1;
   }
#endif

   if(0 == qsee_wstrtostr(w_a, b, 32))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrtostr() failed");
      return -1;
   }

   if(0 != qsee_strnicmp(a, b, 32))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_strnicmp() failed");
      return -1;
   }

   if(0 == qsee_wstrlcat(w_a, w_a, 64))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrlcat() failed");
      return -1;
   }

   if(w_a != qsee_wstrchr(w_a, 'T'))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrchr() failed");
      return -1;
   }
#if 0
   if(NULL != qsee_wstrchr(NULL, 'T'))
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrchr() failed, NULL was accepted as a ptr to a string");
      return -1;
   }
#endif

   qsee_free(a);
   qsee_free(b);
   qsee_free(w_a);

   qsee_log(QSEE_LOG_MSG_DEBUG, "-- COMSTR TESTS passed---------------------------------");
   return 0;
}

int run_tlmm_tests()
{
   uint32_t gpio_key;
   qsee_tlmm_config_t my_config;
   qsee_gpio_value_t myval = 0;
   int index = 0;

   my_config.drive = QSEE_GPIO_6MA;
   my_config.pull = QSEE_GPIO_PULL_UP;
   my_config.direction = QSEE_GPIO_OUTPUT;

   for (index = 0; index < 2; index++)
   {
      if (qsee_tlmm_get_gpio_id("tlmm_gpio_test_pin", &gpio_key) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_get_gpio_id: Failed");
         return -1;
      }
      if (qsee_tlmm_config_gpio_id(gpio_key, &my_config) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_config_gpio_id: Failed");
         return -1;
      }
      if (qsee_tlmm_select_gpio_id_mode(gpio_key, QSEE_GPIO_MODE_GENERAL) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_select_gpio_id_mode to generic IO: Failed");
         return -1;
      }
      if (qsee_tlmm_select_gpio_id_mode(gpio_key, QSEE_GPIO_MODE_PRIMARY) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_select_gpio_id_mode to primary: Failed");
         return -1;
      }

      my_config.pull = QSEE_GPIO_NO_PULL;

      if (qsee_tlmm_select_gpio_id_mode(gpio_key, QSEE_GPIO_MODE_GENERAL) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_select_gpio_id_mode to generic (2nd time): Failed");
         return -1;
      }
      if (qsee_tlmm_gpio_id_out(gpio_key, QSEE_GPIO_HIGH) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_gpio_id_out output-HIGH: Failed");
         return -1;
      }
      if (qsee_tlmm_gpio_id_out(gpio_key, QSEE_GPIO_LOW) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_gpio_id_out output-LOW: Failed");
         return -1;
      }

      my_config.direction = QSEE_GPIO_INPUT;

      if (qsee_tlmm_config_gpio_id(gpio_key, &my_config) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_config_gpio_id: Failed");
         return -1;
      }
      if (qsee_tlmm_gpio_id_in(gpio_key, &myval) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_gpio_id_in signal read: Failed");
         return -1;
      }
      if (qsee_tlmm_release_gpio_id(gpio_key) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_tlmm_release_gpio_id: Failed");
         return -1;
      }
   }
   return 0;

}

static int run_ispi_duplex_ext_test(uint32_t spiDeviceId)
{
   static ISPI_Config_Ext spi_config_ext;
   int ret = 0;
   uint64_t bytes_written = 0;
   size_t bytes_read = 0;
   int i = 0;
   
   spi_config_ext.spiFreq = 19200000;
   spi_config_ext.spiMode = ISPI_MODE_0;
   spi_config_ext.endianness = ISPI_NATIVE;	
   spi_config_ext.csPolarity = ISPI_CS_POLARITY_ACTIVE_HIGH;	
   spi_config_ext.csToggle = ISPI_CS_MODE_KEEP_ASSERTED;
   spi_config_ext.bitsPerWord = 8;	
   spi_config_ext.loopBack = 1;	
   spi_config_ext.slaveIndex = 0;
   spi_config_ext.csClkDelayCycles = 0x15;
   spi_config_ext.interWordDelayCycles = 0;
    
   qsee_log(QSEE_LOG_MSG_DEBUG, "Starting ISPI  Loopback test for device : %d", spiDeviceId);
   
   Object spiSingleton = qsee_open_singleton(CSPI_UID);
   if (Object_isNull(spiSingleton))
   {
       qsee_log(QSEE_LOG_MSG_DEBUG, "run_ispi_duplex_ext_test : qsee_open_singleton FAILED");
       return -1;
   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_open_singleton passed : 0x%x is the singleton",spiSingleton);
   
   if (0 != (ret = ISPI_open(spiSingleton, spiDeviceId)))
   {
      qsee_log(QSEE_LOG_MSG_DEBUG, "run_ispi_duplex_ext_test : ISPI_open FAILED for device %08x , ret = %d\n", spiDeviceId,ret);
      return -1;
   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "ISPI_open passed : device ID %d",spiDeviceId);
   
   for (i = 0; i < SPI_READ_BUF_SIZE; i++) 
   {
      spi_read_data[i] = 0XFF;
   }
   
   if (0 != (ret = ISPI_writeFullDuplexExt(spiSingleton,
                                           (int32_t) spiDeviceId,
                                           &spi_config_ext,
                                           spi_write_data,
                                           SPI_WRITE_BUF_SIZE,
                                           &bytes_written,
                                           spi_read_data,
                                           SPI_READ_BUF_SIZE,
                                           &bytes_read )))
    {
		qsee_log(QSEE_LOG_MSG_DEBUG, "ISPI_writeFullDuplexExt : failed for device %d , ret = %d\n",spiDeviceId,ret);
		return -1;
	}
    
    qsee_log(QSEE_LOG_MSG_DEBUG, "ISPI_writeFullDuplexExt passed : device ID %d",spiDeviceId);
    
    // Compare Write and Read buffers
	for (i=0 ; i < SPI_READ_BUF_SIZE; i++)
  	{
	    if ( spi_write_data[i] != spi_read_data[i] ) 
		{
		    //Comparison failed 
		    qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE SPI Loopback test failed: WR[x]:%d RD[x]:%d, index=%d",spi_write_data[i],spi_read_data[i], i);
		    break;
	    }			
	}
    
    if (i == SPI_READ_BUF_SIZE)
	{
        qsee_log(QSEE_LOG_MSG_DEBUG, "=======ISPI Loopback test SUCCESS for %d bytes=====", i);
    }
    
    if (0 != (ret = ISPI_close(spiSingleton, spiDeviceId)))
    {
       qsee_log(QSEE_LOG_MSG_DEBUG, "run_ispi_duplex_ext_test : ISPI_close FAILED for device %08x , ret = %d\n", spiDeviceId,ret);
       return -1;
    }
    qsee_log(QSEE_LOG_MSG_DEBUG, "ISPI_close passed : device ID %d",spiDeviceId);
    
    return 0;
}

/**
 * @brief     QSEE SPI test
 *               QSEE Sample Client Command ::
 *                    qseecom_sample_client v sampleap 19 1
 *
 * @spiDeviceId    SPI device Id 0 to 11
 *
 */
int run_buses_spi_test(uint32_t spiDeviceId)
{
   int retval = 0;

   qsee_spi_config_t spi_config;
   qsee_spi_transaction_info_t write_info;
   qsee_spi_transaction_info_t read_info;
   int i,j,k;
   unsigned long long  start_time, end_time, total_time;
   unsigned long long  avg_time;
   unsigned int data_sizes[10] = { 1, 32, 64, 128, 256, 512, 1024, 5*1024, 10*1024, 32*1024};
   unsigned int spi_freq[5] = {1000000,10000000, 20000000, 25000000, 50000000};

   qsee_log(QSEE_LOG_MSG_DEBUG, "run_buses_spi_test: spiDeviceId = %d\n", spiDeviceId);
   //START SPI TEST CODE
   spi_config.spi_bits_per_word = 8;
   spi_config.spi_clk_always_on = QSEE_SPI_CLK_NORMAL;
   spi_config.spi_clk_polarity = QSEE_SPI_CLK_IDLE_HIGH;
   spi_config.spi_cs_mode = QSEE_SPI_CS_KEEP_ASSERTED;
   spi_config.spi_cs_polarity = QSEE_SPI_CS_ACTIVE_HIGH;
   spi_config.spi_shift_mode = QSEE_SPI_OUTPUT_FIRST_MODE;
   spi_config.max_freq = 19200000; //19.2 MHz
   spi_config.hs_mode = 0;
   spi_config.loopback = 1;
   spi_config.spi_slave_index = 0;
   spi_config.deassertion_time_ns = 0;


   write_info.buf_addr = (uint8_t*)qsee_malloc(SPI_WRITE_BUF_SIZE);
   if(NULL == write_info.buf_addr)
   {
     qsee_log(QSEE_LOG_MSG_ERROR, "run_buses_spi_test : malloc failed ");
     return -1;
   }
   
   spi_write_data =  write_info.buf_addr;
   for (i = 0; i < SPI_WRITE_BUF_SIZE; i++) {
      spi_write_data[i] = i;
   }   
   write_info.buf_len = SPI_WRITE_BUF_SIZE*sizeof(uint8_t);
   
   read_info.buf_addr = (uint8_t*)qsee_malloc(SPI_READ_BUF_SIZE);
   if(NULL == read_info.buf_addr)
   {
     qsee_free(spi_write_data);
	 
     qsee_log(QSEE_LOG_MSG_ERROR, "run_buses_spi_test : malloc failed ");
     return -1;
   }
   
   spi_read_data =  read_info.buf_addr;
   for (i = 0; i < SPI_READ_BUF_SIZE; i++) {
      spi_read_data[i] = 0XFF;
   }
   read_info.buf_len = SPI_READ_BUF_SIZE*sizeof(uint8_t);

   if (0 != (retval = qsee_spi_open(spiDeviceId))) {
      qsee_log(QSEE_LOG_MSG_ERROR, "qsee_spi_open1: retval = %d\n", retval);
   }

   do{

      //PERFORM WRITE
      if (0 != (retval = qsee_spi_write(spiDeviceId,&spi_config, &write_info))){
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_spi_write: retval = %d\n", retval);
         break;
      }

      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_spi_write: retval = %d\n", retval);

      //PEFORM READ

      if (0 != (retval = qsee_spi_read(spiDeviceId,&spi_config, &read_info))){
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_spi_read: retval = %d\n", retval);
         break;
      }


      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_spi_read: retval = %d\n", retval);


     for(j = 0; j < 5;j++)
     {
        spi_config.max_freq = spi_freq[j]; //19.2 MHz
        for(i =0; i < 10; i++) {
            
            write_info.buf_len = data_sizes[i]*sizeof(uint8_t);
            read_info.buf_len  = data_sizes[i]*sizeof(uint8_t);
            
            for (k = 0; k < SPI_READ_BUF_SIZE; k++) {
               spi_read_data[k] = 0XFF;
            }
            avg_time = 0;
            for(k =0;k<5;k ++) {
               start_time = qsee_get_uptime();
               if (0 != (retval = qsee_spi_full_duplex(spiDeviceId,&spi_config,
                                                       &write_info, &read_info))){
                  qsee_log(QSEE_LOG_MSG_ERROR, "qsee_spi_full_duplex: retval = %d\n",
                           retval);
                  break;
               }
               end_time = qsee_get_uptime();
	           total_time = end_time - start_time;
               avg_time = total_time + avg_time;
            }
            avg_time = avg_time/5;
            qsee_log(QSEE_LOG_MSG_ERROR, "-----TIME TAKEN FOR %d BYTES TRANSFER ON SPI BUS is : %d ms, AT FREQ OF %d ----",data_sizes[i], avg_time, spi_freq[j]);
            
               
            if (retval == -1) {
               break;
            }
            
            		// Compare Write and Read buffers
	        for (k=0 ; k < read_info.buf_len; k++)
  	        {
	      	    if ( spi_write_data[k] != spi_read_data[k] ) 
	      	    {
	      	    	      //Comparison failed 
	      	    	qsee_log(QSEE_LOG_MSG_DEBUG, "QSEE SPI Loopback test failed: WR[x]:%d RD[x]:%d, index=%d",spi_write_data[k],spi_read_data[k], k);
	      	    	break;
	      	    }			
	        }
	      	
	        if (k == read_info.buf_len)
	        {
              qsee_log(QSEE_LOG_MSG_DEBUG, "****QSEE SPI Loopback test SUCCESS for %d bytes*****", k);
              
            }
            else 
            {
                qsee_log(QSEE_LOG_MSG_DEBUG, "****QSEE SPI Loopback test FAILED for %d bytes*****", k);
                break;
            }
        }
      }




   }while(0);


   qsee_free(spi_write_data);
   qsee_free(spi_read_data);
   
   if (0 != (retval = qsee_spi_close(spiDeviceId))) {
      qsee_log(QSEE_LOG_MSG_ERROR, "qsee_spi_close1: retval = %d\n", retval);
   }
   
   if (0 != (retval = run_ispi_duplex_ext_test(spiDeviceId)))
   {
       qsee_log(QSEE_LOG_MSG_ERROR, "run_ispi_duplex_ext_test failed for device %d , retval = %d\n",spiDeviceId, retval);
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "BUSES SPI TEST COMPLETE, retval = %d\n", retval);
   return retval;

}

int run_buses_test(uint32_t I2cDeviceId, uint32_t slaveaddr)
{
   int ret = 0;
   int retClose = 0;
   int i = 0;
   uint8_t                      rd_data[10] = {0};
   uint8_t                      save_data[10] ={0};
   uint8_t                      write_data[10] ={0};
   uint32_t                     buff_size = 10;
   qsee_i2c_config_t            i2c_config;
   qsee_i2c_transaction_info_t  readInfo;
   qsee_i2c_transaction_info_t  writeInfo;
   uint32_t                     eeprom_rd_wr_wait_cycle =10000;

   qsee_log(QSEE_LOG_MSG_DEBUG, "-- BUSES UNIT TESTS ---------------------------------");
   
   qsee_log(QSEE_LOG_MSG_DEBUG, "run_buses_test() entry ! I2cDeviceId ID %d : slave addr = %d \n",I2cDeviceId, slaveaddr);
   
   /* Open the bus. This will lock the bus to TZ */
   ret = qsee_i2c_open(I2cDeviceId);
   if( ret != 0 )
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "qsee_i2c_open() FAILED! I2cDeviceId ID %d : slaveaddr = %d return val = %ld \n",I2cDeviceId, slaveaddr, ret);
      return ret;
   }
   
   qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_i2c_open() PASSED ! I2cDeviceId ID %d : slaveaddr = %d return val = %ld \n",I2cDeviceId, slaveaddr, ret);
   
   for (i = 0; i < buff_size; i++) {
      write_data[i] = i;
   }

   /* This call requires the QUP clocks to be on, or will cause an L2 error */

   do {
      i2c_slave_device_config.slave_address = slaveaddr;	  
      i2c_config.p_bus_config = &i2c_bus_config;
      i2c_config.p_slave_config = &i2c_slave_device_config;
      i2c_slave_device_config.slave_device_type = QSEE_I2C_MEMORY_ADDRESS_DEVICE;

      qsee_log(QSEE_LOG_MSG_DEBUG, "In do while() before read slaveaddr = %d \n",i2c_slave_device_config.slave_address);
     
      readInfo.start_addr = 0x1;
      readInfo.p_buf = save_data;
      readInfo.buf_len = buff_size;
      readInfo.total_bytes = 0;
      
      /*Backing up read data from eeprom*/
      ret = qsee_i2c_read(I2cDeviceId, &i2c_config, &readInfo);
      if( ret != 0 )
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA BACKUP FAILED !!!");
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_i2c_read() FAILED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
         break;
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA BACKUP successful");
         qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_i2c_read() PASSED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
      }
      qsee_spin(eeprom_rd_wr_wait_cycle);
      
      writeInfo.start_addr = 0x1;
      writeInfo.p_buf = write_data;
      writeInfo.buf_len = buff_size;
      writeInfo.total_bytes = 0;
      
      /*Writing data to eeprom*/
      ret = qsee_i2c_write(I2cDeviceId, &i2c_config, &writeInfo);
      if( ret != 0 )
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA WRITE FAILED !!!");
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_i2c_write() FAILED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
         break;
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA WRITE successful");
         qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_i2c_write() PASSED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
      }
      qsee_spin(eeprom_rd_wr_wait_cycle);
      
      readInfo.start_addr = 0x1;
      readInfo.p_buf = rd_data;
      readInfo.buf_len = buff_size;
      readInfo.total_bytes = 0;
      
      /*Reading back written data from eeprom*/
      ret = qsee_i2c_read(I2cDeviceId, &i2c_config, &readInfo);
      if( ret != 0 )
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA READ FAILED !!!");
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_i2c_read() FAILED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
         break;
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA READ successful");
         qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_i2c_read() PASSED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
      }
      
      qsee_spin(eeprom_rd_wr_wait_cycle);
      
      for (i=0 ; i < buff_size; i++)
      {
		if (rd_data[i] != write_data[i]) {
			/* Comparison failed */
			qsee_log(QSEE_LOG_MSG_ERROR, "I2C test failed: i=%d: rd_data[i]=%d: write_data]i]=%d", i, rd_data[i], write_data[i]);
			break;
        }
      }
      if(i == buff_size)
      {
          qsee_log(QSEE_LOG_MSG_ERROR, "****** EEPROM DATA VERIFICATION SUCCESSFUL ********");
      }
      writeInfo.start_addr = 0x1;
      writeInfo.p_buf = save_data;
      writeInfo.buf_len = buff_size;
      writeInfo.total_bytes = 0;

      ret = qsee_i2c_write(I2cDeviceId, &i2c_config, &writeInfo);
      if( ret != 0 )
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA RESTORE FAILED !!!");
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_i2c_write() FAILED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
         break;
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "EEPROM DATA RESTORE successful");
         qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_i2c_write() PASSED! Slaveaddr = %d : Return val = %ld", slaveaddr,ret);
      }
      qsee_spin(eeprom_rd_wr_wait_cycle);
	  
      
   } while (0);

   retClose = qsee_i2c_close(I2cDeviceId);
   if( retClose != 0 )
   {
      qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_i2c_close() FAILED! I2cDeviceId ID %d : Return val = %ld \n",I2cDeviceId, ret);
   }
   else
   {
      qsee_log(QSEE_LOG_MSG_DEBUG, "   qsee_i2c_close() PASSED! I2cDeviceId ID %d : Return val = %ld \n",I2cDeviceId, ret);
   }
   
   i2c_slave_device_config.slave_address = 8;
   
   for(;i2c_slave_device_config.slave_address < 120;i2c_slave_device_config.slave_address++)
   {
     qsee_i2c_open(I2cDeviceId);
     readInfo.start_addr = 0x1;
     readInfo.p_buf = rd_data;
     readInfo.buf_len = 1;
     readInfo.total_bytes = 0;
     i2c_slave_device_config.slave_device_type = QSEE_I2C_DEFAULT_ADDRESS_DEVICE;
     ret = qsee_i2c_read(I2cDeviceId, &i2c_config, &readInfo);
     if (!ret) {
        qsee_log(QSEE_LOG_MSG_DEBUG," Slave found @0x%x Address ",i2c_slave_device_config.slave_address);
        qsee_spin(eeprom_rd_wr_wait_cycle);
     }
     qsee_i2c_close(I2cDeviceId);
   }

   return retClose;
}


int security_state_test()
{
   int ret = 0;
   qsee_secctrl_secure_status_t status;
   memset(&status, 0, sizeof(status));

   ret = qsee_get_secure_state(&status);
   qsee_log(QSEE_LOG_MSG_DEBUG, "  qsee_get_secure_state() pass test: %x %x, ret %d",
            status.value[0], status.value[1], ret);
   if (0 != ret) {
      qsee_log(QSEE_LOG_MSG_ERROR, "qsee_get_secure_state() call fail and returns %d\n", ret);
      return 1;
   }
   if (0 == (status.value[0] & 0x1f)) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "secure boot is setup");
      ret = 0x0;
   } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "secure boot is NOT setup");
      ret = 0x02;
   }

   if (0 == (status.value[0] & 0x20)) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "RPMB is provisioned");
   } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "RPMB boot is NOT provisioned");
      ret |= 0x4;
   }

   return ret;
}


static int run_security_state_test()
{
   int ret = 0;
   qsee_secctrl_secure_status_t status;
   memset(&status, 0, sizeof(status));

   ret = qsee_get_secure_state(&status);
   qsee_log(QSEE_LOG_MSG_DEBUG, "  qsee_get_secure_state() pass(=1): %d, val: %x %x",
            (ret == 0), status.value[0], status.value[1]);

   ret = qsee_get_secure_state(NULL);
   qsee_log(QSEE_LOG_MSG_DEBUG, "  qsee_get_secure_state() negative, NULL buffer, pass(=1): %d", (ret != 0));

   return 0;
}


int run_abort_test(void)
{
   volatile uint32_t* my_bad_ptr = NULL;
   qsee_log(QSEE_LOG_MSG_DEBUG, "Sampleapp about to abort");
   *my_bad_ptr = 5;
   return *my_bad_ptr;
}


static char mybuff[1024];

int run_cache_test()
{

   int testLength = 5;

   qsee_log(QSEE_LOG_MSG_DEBUG, " Test qsee_dcache_clean_region() : addr = %x , length = %d", mybuff, testLength);
   qsee_dcache_clean_region( mybuff, testLength);

   testLength = 6;
   qsee_log(QSEE_LOG_MSG_DEBUG, " Test qsee_dcache_flush_region() : addr = %x , length = %d", mybuff, testLength);
   qsee_dcache_flush_region( mybuff, testLength);

   testLength = 1024;
   qsee_log(QSEE_LOG_MSG_DEBUG, " Test qsee_dcache_inval_region() : addr = %x , length = %d", mybuff, testLength);
   qsee_dcache_inval_region( mybuff, testLength);

   for (int i =0; i<testLength; i++ ){
      mybuff [i] = i%256;
   }

   return 0;
}


int run_sharedbuffer_test(void* data, uint32_t len)
{

   int ret;

  qsee_log(QSEE_LOG_MSG_DEBUG, " Test qsee_register_shared_buffer() : addr = %x , length = %d", data, len);
   ret = qsee_register_shared_buffer(data, len);

   if (ret)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, " qsee_register_shared_buffer() test failed");
      return ret;
   }

  qsee_log(QSEE_LOG_MSG_DEBUG, " Test  qsee_deregister_shared_buffer: addr = %x , length = %d", data, len);
   ret = qsee_deregister_shared_buffer(data);

   if (ret)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, " qsee_deregister_shared_buffer() test failed");
      return ret;
   }

   return 0;
}

//------------------------------------------------------------------------------
//  internal supporting function: read_debug_policy
//------------------------------------------------------------------------------
static int read_debug_policy(void *address, unsigned int size, size_t *debugPolicyOutSize)
{
  int debugPolicyCallReturnCode = Object_ERROR;
  Object debugPolicyObject = qsee_open_singleton(CDebugPolicySource_UID);

  if (!Object_isNull(debugPolicyObject))
  {
    /* The returned buffer size only matters for successful call. It could be invalid value,
       When the interface call returns failure. */
    debugPolicyCallReturnCode = ISource_read(debugPolicyObject, address, size, debugPolicyOutSize);
  }
  else
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "read_debug_policy Failed: Object is NULL!!!");
  }

  return debugPolicyCallReturnCode;
}

//------------------------------------------------------------------------------
//  run_debugpolicy_test_neg (negative test)
//------------------------------------------------------------------------------
int run_debugpolicy_test_neg()
{
  int ret;
  size_t debugPolicyOutSize = 0;
  uint8_t * debugPolicyBufPtr = (uint8_t*)qsee_malloc(TEST_DEBUGPOLICY_BUF_SIZE);
  if(NULL == debugPolicyBufPtr)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Out of memory.");
    return -1;
  }

  // still allocate the same buffer to pass in the call, as positive test. to
  // ensure only size = 0 causing the failure.
  qsee_log(QSEE_LOG_MSG_DEBUG, "debugpolicy test: neg - buf size too small");
  ret = read_debug_policy(debugPolicyBufPtr, 0, &debugPolicyOutSize);
  /* Null pointer check is handled by MINK framework and test case is handled
     by Mink IPC framewok. */
  qsee_free(debugPolicyBufPtr);
  TEST_ASSERT(ret, Object_ERROR_SIZE_OUT, "Object_ERROR_SIZE_OUT expected");

  return 0;
}

//------------------------------------------------------------------------------
//  run_debugpolicy_test_sanity (when debug policy is not loaded)
//------------------------------------------------------------------------------
int run_debugpolicy_test_sanity()
{
  int ret;
  size_t debugPolicyOutSize = 0;
  uint8_t * debugPolicyBufPtr = (uint8_t*)qsee_malloc(TEST_DEBUGPOLICY_BUF_SIZE);
  if(NULL == debugPolicyBufPtr)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Out of memory.");
    return -1;
  }
  memset(debugPolicyBufPtr, 0, TEST_DEBUGPOLICY_BUF_SIZE);

  qsee_log(QSEE_LOG_MSG_DEBUG, "debugpolicy test: posi addr = %x , length = %d", debugPolicyBufPtr, TEST_DEBUGPOLICY_BUF_SIZE);
  ret = read_debug_policy(debugPolicyBufPtr, TEST_DEBUGPOLICY_BUF_SIZE, &debugPolicyOutSize);
  qsee_free(debugPolicyBufPtr);
  TEST_ASSERT(ret, Object_ERROR_INVALID, "debug policy should be not loaded");

  /* neg test */
  TEST_ASSERT(run_debugpolicy_test_neg(), 0, "neg test should pass");

  return 0;
}

//------------------------------------------------------------------------------
//  run_debugpolicy_test (includes positive test case requiring debug policy loaded)
//------------------------------------------------------------------------------
int run_debugpolicy_test()
{
  int ret;
  size_t debugPolicyOutSize = 0;
  uint8_t * debugPolicyBufPtr = (uint8_t*)qsee_malloc(TEST_DEBUGPOLICY_BUF_SIZE);
  if(NULL == debugPolicyBufPtr)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Out of memory.");
    return -1;
  }
  memset(debugPolicyBufPtr, 0, TEST_DEBUGPOLICY_BUF_SIZE);

  qsee_log(QSEE_LOG_MSG_DEBUG, "debugpolicy test: posi addr = %x , length = %d", debugPolicyBufPtr, TEST_DEBUGPOLICY_BUF_SIZE);
  ret = read_debug_policy(debugPolicyBufPtr, TEST_DEBUGPOLICY_BUF_SIZE, &debugPolicyOutSize);

  if (Object_OK != ret) {
    qsee_log(QSEE_LOG_MSG_ERROR, "Failed! (0x%x != 0x%x) debug policy should be loaded", ret, Object_OK);
    qsee_free(debugPolicyBufPtr);
    return -1;
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Success! (0x%x == 0x%x) debug policy should be loaded", ret, Object_OK);
  }

  /* check output size larger than the required minimum, only valid for successful call */
  if (debugPolicyOutSize < TEST_DEBUGPOLICY_FILE_SIZE)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "Failed! returned debug policy size (0x%x) < the required (0x%x).",
             debugPolicyOutSize, TEST_DEBUGPOLICY_FILE_SIZE);
    qsee_free(debugPolicyBufPtr);
    return -1;
  }

  /* check magic number */
  ret = memcmp(debugPolicyBufPtr, "DBGP", 4);
  qsee_free(debugPolicyBufPtr);
  TEST_ASSERT(ret, 0, "debug policy magic number: 'DBGP'");

  /* neg test */
  TEST_ASSERT(run_debugpolicy_test_neg(), 0, "neg test should pass");
  return 0;
}

int run_securechannel_test(void* data, uint32_t len)
{

   int ret;

   char data_buff[] = "ABRACADABRA";
   int buffLen = sizeof(data_buff);
   int output_buff_size = buffLen + 100;

   char * output_msg_ptr = (char*)qsee_malloc(output_buff_size);
   uint32_t output_msg_len_ptr = output_buff_size;

   qsee_log(QSEE_LOG_MSG_DEBUG, " Test qsee_secure_message() : addr = %x , length = %d", data, len);

   ret = qsee_secure_message (1,2, (const uint8_t *) data_buff, buffLen, (uint8_t *) output_msg_ptr, &output_msg_len_ptr);

   if (ret)
   {
      qsee_log(QSEE_LOG_MSG_ERROR, " qsee_secure_message() test failed");
      return ret;
   }
   else
   {
   qsee_log(QSEE_LOG_MSG_DEBUG, "------------- qsee_secure_message() test passed------------");
   }

   qsee_free(output_msg_ptr);

// Add test for qsee_authenticate_decrypt_message here for completeness. The message to pass should have proper header.

   return 0;
}

static int get_ns_vbar_el1_system_register(uint32_t sr, uint64_t* val)
{
  int err;
  Object CNSSystemRegObj = Object_NULL;

  err = qsee_open(CNSSystemReg_UID, &CNSSystemRegObj);

  if(Object_isERROR(err)){
    return err;
  }

  if (Object_isNull(CNSSystemRegObj)) {
    return Object_ERROR;
  }

  err = INSSystemReg_GetNSSystemReg(CNSSystemRegObj,
                                        sr,
                                        val);

  return err;
}

int run_el2_rtic_report_test(uint32_t report_header_expected, uint32_t protection_enabled_expected)
{
  int err;
  IRTICReport_reportHeader report_header;
  uint32_t PFCounters[IRTICReport_PF_COUNTER_END];
  size_t PFCounters_Out_Len = 0;
  uint8_t MPHash[QSEE_SHA256_HASH_SZ];
  size_t MPHash_Out_Len = 0;

  memset(&report_header, 0, sizeof(report_header));
  memset(PFCounters, 0, sizeof(PFCounters));
  memset(MPHash, 0, QSEE_SHA256_HASH_SZ);

  Object CRTICReportObj = Object_NULL;

  err = qsee_open(CRTICReport_UID, &CRTICReportObj);

  if(Object_isERROR(err)){
    qsee_log(QSEE_LOG_MSG_DEBUG,
              "****FAILED : EL2 RTIC Test. qsee_open failed!");
    return err;
  }

  if (Object_isNull(CRTICReportObj)) {
    qsee_log(QSEE_LOG_MSG_DEBUG,
              "****FAILED : EL2 RTIC Test. qsee_open returned NULL Object!");
    err = Object_ERROR;
    goto exit;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Getting Report Header");
  qsee_log(QSEE_LOG_MSG_DEBUG, "---------------------");
  err = IRTICReport_getReportHeader(CRTICReportObj,
                                       &report_header);

  if(Object_isERROR(err) && report_header_expected){
    qsee_log(QSEE_LOG_MSG_DEBUG,
              "****FAILED : EL2 RTIC Test. IRTICReport_getReportHeader failed! err=%d", err);
    goto exit;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Report Version: %u", report_header.reportVersion);
  qsee_log(QSEE_LOG_MSG_DEBUG, "MP Catalog Version: %u", report_header.mpCatalogVersion);
  qsee_log(QSEE_LOG_MSG_DEBUG, "Total Number of Incidents: %u", report_header.numIncidents);
  qsee_log(QSEE_LOG_MSG_DEBUG, "Protection Enabled: %u", report_header.protectionEnabled);

  if ((!report_header.reportVersion) && report_header_expected)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG,
              "****FAILED : EL2 RTIC Test. Unexpected report version! report_header.reportVersion=0x%x", report_header.reportVersion);
    err = -1;
    goto exit;
  }

  if (report_header.protectionEnabled != (uint8_t)protection_enabled_expected)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG,
              "****FAILED : EL2 RTIC Test. Unexpected protection enabled value in report header! report_header.protectionEnabled=0x%x, protection_enabled_expected=0x%x", report_header.protectionEnabled, (uint8_t)protection_enabled_expected);
    err = -1;
    goto exit;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Getting PF Counters");
  qsee_log(QSEE_LOG_MSG_DEBUG, "-------------------");
  err = IRTICReport_getReportPFCounters(CRTICReportObj, PFCounters, IRTICReport_PF_COUNTER_END * sizeof(uint32_t), &PFCounters_Out_Len);

  if(Object_isERROR(err))
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "****FAILED : EL2 RTIC Test. IRTICReport_getReportPFCounters failed! err=%d", err);
    goto exit;
  }

  for (int i=0; i<IRTICReport_PF_COUNTER_END; i++)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "PFCounter %u has value %u", i, PFCounters[i]);
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Getting MPHash");
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------");
  err = IRTICReport_getMPHash(CRTICReportObj, MPHash, QSEE_SHA256_HASH_SZ, &MPHash_Out_Len);

  if(Object_isERROR(err))
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "****FAILED : EL2 RTIC Test. IRTICReport_getMPHash failed! err=%d", err);
    goto exit;
  }

  // Log MPHash
  for (int i=0; i<MPHash_Out_Len;)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "MPHash has value 0x%1x,0x%1x,0x%1x,0x%1x", (unsigned int)MPHash[i], (unsigned int)MPHash[i+1], (unsigned int)MPHash[i+2], (unsigned int)MPHash[i+3]);
    i=i+4;
  }


  qsee_log(QSEE_LOG_MSG_DEBUG,
              "****PASSED : EL2 RTIC Test.");
exit:
  Object_RELEASE_IF(CRTICReportObj);
  return 0;
}

int run_get_sysreg_test(uint32_t sr, uint64_t sysreg_exp_val)
{
   uint64_t sysreg_val = 0;

   int retval = get_ns_vbar_el1_system_register(sr, &sysreg_val);

   if (retval || (sysreg_exp_val != sysreg_val)) {
     qsee_log(QSEE_LOG_MSG_DEBUG,
              "****FAILED : Get NS System Register Test. retval:%d, exp sysreg value:0x%08x and read sysreg value:0x%08x", retval, sysreg_exp_val, sysreg_val);
     return -1;
   } else {
     qsee_log(QSEE_LOG_MSG_DEBUG,
              "****PASSED : Get NS System Register Test. retval:%d, expsysreg:0x%x and readsysreg:0x%x", retval, sysreg_exp_val, sysreg_val);
     return 0;
   }
}

int run_trans_test(qsee_transm_t tt, uintptr_t va, uintptr_t exp_pa)
{
   uintptr_t pa = 0;

   int retval = qsee_trans_ns_addr(tt, va, &pa);

   if (retval || (exp_pa & 0xFFFFF000) != pa) {
      /* Bit one indicates an expected translation fault. */
      if (QSEE_TRANS_NS_ADDR_ERROR_TRANSLATION_FAULT == retval && (exp_pa & 1)) {
         if (QSEE_EL1_READ == tt) {
            qsee_log(QSEE_LOG_MSG_DEBUG,
                     "****PASSED : NS Address Translation Test - Read 0x%08x Fault Result", va);
         } else {
            qsee_log(QSEE_LOG_MSG_DEBUG,
                     "****PASSED : NS Address Translation Test - Write 0x%08x Fault Result", va);
         }
         return 0;
      } else {
         if (QSEE_EL1_READ == tt) {
            qsee_log(QSEE_LOG_MSG_ERROR,
                     "****FAILED : NS Address Translation Test - Read 0x%08x Expected 0x%08x Result 0x%08x Return %d", va , exp_pa, pa, retval);
         } else {
            qsee_log(QSEE_LOG_MSG_ERROR,
                     "****FAILED : NS Address Translation Test - Write 0x%08x Expected 0x%08x Result 0x%08x Return %d", va, exp_pa, pa, retval);
         }
         return -1;
      }
   } else {
      if (QSEE_EL1_READ == tt) {
         qsee_log(QSEE_LOG_MSG_DEBUG,
                  "****PASSED : NS Address Translation Test - Read 0x%08x", va);
      } else {
         qsee_log(QSEE_LOG_MSG_DEBUG,
                  "****PASSED : NS Address Translation Test - Write 0x%08x", va);
      }
      return 0;
   }
}


static int test_write_and_read_no_offset(size_t oemBufLen)
{
   int ret = FAILED;
   char *buf = NULL;
   char *cmpbuf = NULL;
   size_t bufLen = oemBufLen;
   buf = qsee_zalloc(bufLen);
   cmpbuf = qsee_zalloc(bufLen);
   if (!buf || !cmpbuf) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_zalloc() FAILED");
      goto bail;
   }

   qsee_strlcpy(buf, "test write and read", bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, strlen(buf)));
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0, cmpbuf, strlen(buf)));
   ut_assert_eqi(0, memcmp(buf, cmpbuf, bufLen));

   memset(buf, 0, bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, bufLen));
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0, cmpbuf, bufLen));
   ut_assert_eqi(0, memcmp(buf, cmpbuf, bufLen));

   qsee_strlcpy(buf, "test", bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, strlen(buf)));
   qsee_strlcpy(buf, "test overwrite", bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, strlen(buf)));
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0, cmpbuf, strlen(buf)));
   ut_assert_eqi(0, memcmp(buf, cmpbuf, strlen(buf)));

   ret = SUCCESS;

bail:
   if (buf) {
      qsee_free(buf);
   }
   if (cmpbuf) {
      qsee_free(cmpbuf);
   }
   return ret;
}

static int test_write_and_read_with_offset(size_t oemBufLen)
{
   int ret = FAILED;
   char *buf = NULL;
   char *cmpbuf = NULL;
   size_t bufLen = oemBufLen;
   buf = qsee_zalloc(bufLen);
   cmpbuf = qsee_zalloc(bufLen);
   if (!buf || !cmpbuf) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_zalloc() FAILED");
      goto bail;
   }

   qsee_strlcpy(buf, "test offset write and read", bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0x50, buf, strlen(buf)));
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0x50, cmpbuf, strlen(buf)));
   ut_assert_eqi(0, memcmp(buf, cmpbuf, strlen(buf)));

   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0x50, cmpbuf, strlen("test")));
   ut_assert_eqi(0, memcmp(buf, cmpbuf, strlen("test")));

   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0x50 + strlen("test "), cmpbuf, strlen("offset")));
   ut_assert_eqi(0, memcmp(buf + strlen("test "), cmpbuf, strlen("offset")));

   memset(buf, 0, bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, bufLen));
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0, cmpbuf, bufLen));
   ut_assert_eqi(0, memcmp(buf, cmpbuf, bufLen));

   ret = SUCCESS;

bail:
   if (buf) {
      qsee_free(buf);
   }
   if (cmpbuf) {
      qsee_free(cmpbuf);
   }
   return ret;
}

static int test_read_size_greater_than_buf(size_t oemBufLen)
{
   int ret = FAILED;
   char *buf = NULL;
   char *cmpbuf = NULL;
   size_t bufLen = oemBufLen;
   size_t cmpbufLen = bufLen * 2;

   buf = qsee_zalloc(bufLen);
   cmpbuf = qsee_zalloc(cmpbufLen);
   if (!buf || !cmpbuf) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_zalloc() FAILED");
      goto bail;
   }

   /* Start with known OEM buffer of all zeros */
   memset(buf, 0, bufLen);
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, bufLen));

   /* Initialize local buffers */
   qsee_strlcpy(buf, "test large read buffer", bufLen);
   memset(cmpbuf, 0xFF, cmpbufLen);

   /* Write string into OEM buffer. */
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_write_oem_buffer(0, buf, strlen(buf)));
   ut_assert_eqi(QSEE_OEM_BUFFER_SUCCESS, qsee_read_oem_buffer(0, cmpbuf, cmpbufLen));

   /* Check entire buffer copied correctly */
   ut_assert_eqi(0, memcmp(buf, cmpbuf, bufLen));

   /* Check remaining part of input buffer was unchanged. */
   memset(buf, 0xFF, bufLen);
   ut_assert_eqi(0, memcmp(cmpbuf + bufLen, buf, cmpbufLen - bufLen));

   ret = SUCCESS;

bail:
   if (buf) {
      qsee_free(buf);
   }
   if (cmpbuf) {
      qsee_free(cmpbuf);
   }
   return ret;
}

static int test_overflow_failures(void)
{
   /* AA32 cannot overflow.  Cannot test overflow for AA64 on target
      from EL0 due to MINK IPC validating buffer arguments as coming
      from the address space of the application. */
   return SUCCESS;
}

static int test_bad_offset_failures(size_t oemBufLen)
{
   int ret = FAILED;
   char *buf = NULL;
   size_t bufLen = oemBufLen;
   buf = qsee_zalloc(bufLen);
   if (!buf) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_zalloc() FAILED");
      goto bail;
   }

   qsee_strlcpy(buf, "test bad offsets", bufLen);

   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_OFFSET_OUT_OF_BOUNDS, qsee_write_oem_buffer(bufLen, buf, 0));
   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_OFFSET_OUT_OF_BOUNDS, qsee_write_oem_buffer(bufLen * 2, buf, bufLen));

   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_OFFSET_OUT_OF_BOUNDS, qsee_read_oem_buffer(bufLen, buf, 0));
   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_OFFSET_OUT_OF_BOUNDS, qsee_read_oem_buffer(bufLen * 5, buf, bufLen));

   ret = SUCCESS;

bail:
   if (buf) {
      qsee_free(buf);
   }

   return ret;
}

static int test_bad_source_buffer_failures(size_t oemBufLen)
{
   int ret = FAILED;
   char *buf = NULL;
   size_t bufLen = 2 * oemBufLen;

   buf = qsee_zalloc(bufLen);
   if (!buf) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_zalloc() FAILED");
      goto bail;
   }

   qsee_strlcpy(buf, "test bad source buffers", bufLen);

   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_SOURCE_BUFFER_TOO_LARGE, qsee_write_oem_buffer(0, buf, bufLen));
   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_SOURCE_BUFFER_TOO_LARGE, qsee_write_oem_buffer(oemBufLen - 1, buf, 2));
   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_SOURCE_BUFFER_TOO_LARGE, qsee_write_oem_buffer(0, buf, oemBufLen + 1));
   ut_assert_eqi( QSEE_OEM_BUFFER_ERROR_SOURCE_BUFFER_TOO_LARGE, qsee_write_oem_buffer(0, buf, bufLen));

   ret = SUCCESS;

bail:
   if (buf) {
      qsee_free(buf);
   }

   return ret;
}

int run_oem_buf_test(size_t requestedOemBufLen, bool requestedOemBufLenValid)
{
   int ret = FAILED;
   size_t oemBufLen = qsee_get_oem_buffer_length();

   if (requestedOemBufLenValid) {
      if (oemBufLen != requestedOemBufLen) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_get_oem_buffer_length() oemBufLen size for chipset not as expected: %zu vs %zu", oemBufLen, requestedOemBufLen);
         goto bail;
      }
      if (oemBufLen == 0) {
         qsee_log(QSEE_LOG_MSG_DEBUG, "OEM Buf not supported on this chipset PASSED");
         ret = SUCCESS;
         goto bail;
      }
   }
   else {
      if (0 == oemBufLen) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_get_oem_buffer_length() returned 0, OEMBuf might not be supported on this chipset! FAILED?");
         goto bail;
      }
      else if (TZBSP_OEM_IMEM_MIN_SIZE > oemBufLen) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_get_oem_buffer_length() returned a size smaller than the minimnum supported: %zu vs %u", oemBufLen, TZBSP_OEM_IMEM_MIN_SIZE);
         goto bail;
      }
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_get_oem_buffer_length() PASSED");

   ut_assert_eqi(SUCCESS, test_write_and_read_no_offset(oemBufLen));
   qsee_log(QSEE_LOG_MSG_DEBUG, "test_write_and_read_no_offset() PASSED");
   ut_assert_eqi(SUCCESS, test_write_and_read_with_offset(oemBufLen));
   qsee_log(QSEE_LOG_MSG_DEBUG, "test_write_and_read_with_offset() PASSED");
   ut_assert_eqi(SUCCESS, test_read_size_greater_than_buf(oemBufLen));
   qsee_log(QSEE_LOG_MSG_DEBUG, "test_read_size_greater_than_buf() PASSED");

   ut_assert_eqi(SUCCESS, test_overflow_failures());
   qsee_log(QSEE_LOG_MSG_DEBUG, "test_overflow_failures() PASSED");
   ut_assert_eqi(SUCCESS, test_bad_offset_failures(oemBufLen));
   qsee_log(QSEE_LOG_MSG_DEBUG, "test_bad_offset_failures() PASSED");
   ut_assert_eqi(SUCCESS, test_bad_source_buffer_failures(oemBufLen));
   qsee_log(QSEE_LOG_MSG_DEBUG, "test_bad_source_buffer_failures() PASSED");

   ret = SUCCESS;
bail:
   return ret;
}

extern int run_smplserv_test(void);

int run_intlock_tests(void)
{
  int retval = QSEE_INTMASK_ERROR;
  qsee_intsrc_t ori_mask;
  qsee_intsrc_t restore_mask = QSEE_INTSRC_ENUM;
  qsee_intsrc_t cur_mask = QSEE_INTSRC_ENUM;

  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  ori_mask = cur_mask;

  qsee_get_oem_buffer_length();
  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  TEST_ASSERT(cur_mask, ori_mask, "Check intmask after syscall");

  retval = qsee_disable_all_interrupts(&restore_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_disable_all_interrupts() check @ line %u", __LINE__);
  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  TEST_ASSERT(restore_mask, ori_mask,
              "Check restore_mask from qsee_disable_all_interrupts()");
  if (sizeof(uint64_t) == sizeof(uintptr_t)) {
    TEST_ASSERT(cur_mask, QSEE_FIQ | QSEE_IRQ | QSEE_SERROR | QSEE_DEBUG,
                "Check interrupt mask after qsee_disable_all_interrupts()");
  }
  else {
    TEST_ASSERT(cur_mask, QSEE_FIQ | QSEE_IRQ | QSEE_SERROR,
                "Check interrupt mask after qsee_disable_all_interrupts()");
  }

  qsee_get_oem_buffer_length();
  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  if (sizeof(uint64_t) == sizeof(uintptr_t)) {
    TEST_ASSERT(cur_mask, QSEE_FIQ | QSEE_IRQ | QSEE_SERROR | QSEE_DEBUG,
                "Check all interrupts stay disabled after syscall");

  }
  else {
    TEST_ASSERT(cur_mask, QSEE_FIQ | QSEE_IRQ | QSEE_SERROR,
                "Check all interrupts stay disabled after syscall");
  }

  retval = qsee_set_intmask(restore_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_set_intmask() check @ line %u", __LINE__);
  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  TEST_ASSERT(cur_mask, ori_mask,
              "Check qsee_set_intmask() to restore value");

  retval = qsee_set_intmask(cur_mask | QSEE_IRQ);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_set_intmask() check @ line %u", __LINE__);
  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  TEST_ASSERT(cur_mask, ori_mask | QSEE_IRQ,
              "Check qsee_set_intmask() masking one interrupt source");

  run_smplserv_test();
  retval = qsee_get_intmask(&cur_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
  TEST_ASSERT(cur_mask, ori_mask | QSEE_IRQ,
             "Check intmask after making call to another secure application");

  retval = qsee_set_intmask(ori_mask);
  TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_set_intmask() check @ line %u", __LINE__);

#if !defined(OFFTARGET)
  /* If aa64, we can set DAIF directly! */
  if (sizeof(uint64_t) == sizeof(uintptr_t)) {
    uint64_t mask64;
    __asm volatile ("mrs %0, DAIF" : "=r" (mask64) :);
    TEST_ASSERT(mask64 >> 6, ori_mask,
                "Check mrs read of interrupt mask shows expected value");

    retval = qsee_set_intmask(ori_mask | QSEE_IRQ);
    TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_set_intmask() check @ line %u", __LINE__);
    __asm volatile ("mrs %0, DAIF" : "=r" (mask64) :);
    TEST_ASSERT(mask64 >> 6, ori_mask | QSEE_IRQ,
               "Check mrs read of interrupt mask shows expected value");

    uint64_t daif = 0x80;
    __asm volatile ("msr DAIF, %x0" : : "r" (daif));
    __asm volatile ("mrs %0, DAIF" : "=r" (mask64) :);
    TEST_ASSERT(mask64, daif, "Check msr write then mrs read");

    retval = qsee_get_intmask(&cur_mask);
    TEST_ASSERT(retval, QSEE_INTMASK_SUCCESS, "qsee_get_intmask() check @ line %u", __LINE__);
    TEST_ASSERT(cur_mask, daif >> 6,
                "Check msr write then qsee_get_intmask() read");
  }
#endif

  return SUCCESS;
}

int run_log_bitmask_test()
{
   uint8_t pri = 0;
   uint8_t expected = QSEE_LOG_MSG_LOW | QSEE_LOG_MSG_HIGH;
   uint8_t current = qsee_log_get_mask();

   qsee_log_set_mask(QSEE_LOG_MSG_LOW);
   qsee_log(QSEE_LOG_MSG_LOW, "-- LOG BIT MASK TEST LOW ---------------------------------");
   qsee_log(QSEE_LOG_MSG_MED, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_HIGH, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_ERROR, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_FATAL, "-- ALERT: Should Not see this - run_log_bitmask_test----");

   qsee_log_set_mask(QSEE_LOG_MSG_MED);
   qsee_log(QSEE_LOG_MSG_LOW, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_MED, "-- LOG BIT MASK TEST MED ---------------------------------");
   qsee_log(QSEE_LOG_MSG_HIGH, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_ERROR, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_FATAL, "-- ALERT: Should Not see this - run_log_bitmask_test----");

   qsee_log_set_mask(QSEE_LOG_MSG_HIGH);
   qsee_log(QSEE_LOG_MSG_LOW, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_MED, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_HIGH, "-- LOG BIT MASK TEST HIGH ---------------------------------");
   qsee_log(QSEE_LOG_MSG_ERROR, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_FATAL, "-- ALERT: Should Not see this - run_log_bitmask_test----");

   qsee_log_set_mask(QSEE_LOG_MSG_ERROR);
   qsee_log(QSEE_LOG_MSG_LOW, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_MED, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_HIGH, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_ERROR, "-- LOG BIT MASK TEST ERROR ---------------------------------");
   qsee_log(QSEE_LOG_MSG_FATAL, "-- ALERT: Should Not see this - run_log_bitmask_test----");

   qsee_log_set_mask(QSEE_LOG_MSG_FATAL);
   qsee_log(QSEE_LOG_MSG_LOW, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_MED, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_HIGH, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_ERROR, "-- ALERT: Should Not see this - run_log_bitmask_test----");
   qsee_log(QSEE_LOG_MSG_FATAL, "-- LOG BIT MASK TEST FATAL ---------------------------------");

   pri = qsee_log_get_mask();

   qsee_log(QSEE_LOG_MSG_HIGH, "Mask Value set = %0x", pri);

   qsee_log_set_mask(QSEE_LOG_MSG_LOW | QSEE_LOG_MSG_HIGH);
   pri = qsee_log_get_mask();

   TEST_ASSERT(pri, expected, "Priority should be value of %d", expected);
   qsee_log(QSEE_LOG_MSG_HIGH, "Mask Value set = %0x", pri);
   qsee_log_set_mask(current);
   return SUCCESS;
}
