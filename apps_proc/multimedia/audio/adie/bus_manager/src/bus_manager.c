/**
@file bus_manager.c
@brief
Implementation of the bus manager for WCD
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                           Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/bus_manager/src/bus_manager.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/30/13   AG      SAL warning fixes after logging is enabled in debug mode
09/02/13   SP      Fix for ADIE RTC AHB register read functionality
03/02/13   KNM,SP  Refactored bus manager read/write interface to suit both
                   I2C & Slimbus.
12/15/10   SK	   Initial revision
===============================================================================*/

#include "bus_manager.h"
#include "bus_manager_target.h"

#ifdef SPL_LOG_SUPPORT
#include "bus_manager.tmh"
#endif

//comment in to enable debug logging for easy register debugging/logging analysis
//In Notepad++ do a search for "=WCD_RDL" in the current document
//Then paste the results in anotehr file and use alt+shift to select the register writes
//allows easy comparison with the QCRG
//This will print out all register writes in the format: 0xREG, 0xMASK, 0xSHIFTED_VALUE
#define REGISTERS_DEBUG_LOGGING

extern wcd_bool WCD_G_FALSE_BOOLEAN;

#define RETRY_READS_AMOUNT  3
#define RETRY_WRITES_AMOUNT 3
#define RETRY_DELAY_MS      2


typedef struct
{
    wcd_bus_type  bus_type;
    void*         gh_bus_mutex;
    uint32         bus_activate_count;
    uint32        init_counter;
}wcd_bus_context_t;

static wcd_bool  is_flush_required = WCD_FALSE;
static wcd_bool  force_flush_write = WCD_FALSE;

static wcd_bus_context_t *context = NULL;

/**
* \brief Integers containing if registers are noncachable or not
*/
static uint8* noncachable_registers = NULL;

/**
* \brief Shadow Register
*/
static uint8* shadow_register = NULL;
wcd_bool is_shadow_register_initialized = WCD_FALSE;



/*
* \brief Initializes the bus with the bus handle passed in.
* @param bus_type == E_WCD_SLIMBUS, slimbus is used, E_WCD_I2C, I2C/I2S is used
*/
wcd_result wcd_bus_init(wcd_bus_type bus_type)
{
    wcd_result rc = E_WCD_BUS_ERROR;
    WCD_DEBUG_INFO_0("+WCD_BUS_MGR: <Bus init.>\n");

    if (context)
    {
        //bus service context exist, return success
        context->init_counter++;
        rc = E_WCD_SUCCESS;
    }
    else
    {
        context = (wcd_bus_context_t*)WCD_MALLOC(sizeof(wcd_bus_context_t), WCD_MALLOC_START_TAG);

        //if bus service context was created correctly and it was recently created, initialize
        if (context)
        {
            //context initialization
            WCD_MEMSET(context, 0, sizeof(wcd_bus_context_t));
            context->bus_type = bus_type;
            context->init_counter = 1;
            context->bus_activate_count = 0;
            context->gh_bus_mutex = WCD_CREATE_MUTEX();

            if (context->gh_bus_mutex != NULL)
            {
                rc = wcd_osal_initialize_bus(context->bus_type);
                if (rc != E_WCD_SUCCESS)
                {
                    WCD_DEBUG_ERROR_0("=WCD_BUS_MGR:<Error initializing bus.>\n");
                }
            }// end context->gh_bus_mutex

            if (rc == E_WCD_SUCCESS)
            {
                WCD_DEBUG_INFO_1("=WCD_BUS_MGR:<trying to initialize physical memory for shadow register size %d.>\n", rc);
                is_shadow_register_initialized = WCD_FALSE;
                //get at least WCD_NUMBER_OF_REGISTERS+WCD_NUMBER_OF_REGISTERS/8
                shadow_register = (uint8*)WCD_PMEM_MALLOC((WCD_NUMBER_OF_REGISTERS + (WCD_NUMBER_OF_REGISTERS >> 3)));
                rc = E_WCD_MEMORY_ALLOCATION_ERROR;

                if (shadow_register)
                {
                    rc = E_WCD_SUCCESS;
                }
            }

            if (rc != E_WCD_SUCCESS)
            {
                wcd_bus_deinit();
            }

        } //service context was created if
    }//if service context does not exist if

    WCD_DEBUG_INFO_1("-WCD_BUS_MGR: <Bus init. RC:%d.>\n", rc);

    return rc;
}

wcd_result wcd_bus_deinit(void)
{
    wcd_result rc = E_WCD_SUCCESS;
    WCD_DEBUG_INFO_0("+WCD_BUS_MGR: <Bus deinit.>\n");

    if (context)
    {
        context->init_counter--;
        if (context->init_counter != 0)
        {
      WCD_DEBUG_INFO_1("=WCD_BUS_MGR: <Init count is: %u.>\n", (unsigned int)context->init_counter);
            rc = E_WCD_SUCCESS;
        }
        else
        {
            wcd_osal_deinitialize_bus();
            WCD_DESTROY_MUTEX(context->gh_bus_mutex);
            WCD_FREE(context, WCD_MALLOC_START_TAG + 0);
            if (shadow_register)
            {
                WCD_PMEM_FREE(shadow_register);
                shadow_register = NULL;
                noncachable_registers = NULL;
                is_shadow_register_initialized = WCD_FALSE;
            }
            context = NULL;
            rc = E_WCD_SUCCESS;
        }
    }

    WCD_DEBUG_INFO_1("-WCD_BUS_MGR: <Bus deinit. RC:%d.>\n", rc);
    return rc;
}

wcd_result wcd_register_write(wcd_ctrl_bus_interface ctrl_bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 value)
{
    wcd_result rc = E_WCD_BUS_ERROR;
    wcd_result mutex_rc = E_WCD_OS_CALL_ERROR;
    int32 wait_result = WCD_WAIT_DONE;

    uint8 temp_read = 0;
    uint8 actual_write = 0;

    uint8 noncachable_index = 0;
    uint8 noncachable_bit = 0;

    uint8 retry_count = 0;

    wcd_bool noncachable = WCD_TRUE;

    //The HAL generator has some 0/0/0 register writes for other purposes. These should be ignored.
    if (reg == 0 && mask == 0 && shift == 0 && value == 0)
    {
        return E_WCD_SUCCESS;
    }

  WCD_DEBUG_WARN_0("+WCD_BUS_MGR: <Register write.>\n");
  WCD_DEBUG_WARN_1("=WCD_BUS_MGR: <Register write: Interface: %d.>\n", ctrl_bus_intf);
  WCD_DEBUG_WARN_3("=WCD_BUS_MGR: <Register write: 0x%x , Mask: 0x%x , shifted value: 0x%x >\n", reg, mask, value<<shift);
#ifdef REGISTERS_DEBUG_LOGGING
    WCD_DEBUG_WARN_3("=WCD_RDL: <0x%x, 0x%x, 0x%x >\n", reg, mask, value << shift);
#endif

    //wait for mutex
    if (context)
    {
        wait_result = WCD_WAIT_FOR_MUTEX(context->gh_bus_mutex, WCD_INFINITE);
        if (WCD_WAIT_DONE == wait_result)
        {
            mutex_rc = E_WCD_SUCCESS;
        }

        if (mutex_rc == E_WCD_SUCCESS)
        {
            //initially mark RC as success, assuming activate count is good
            rc = E_WCD_SUCCESS;

            if (context->bus_activate_count == 0)
            {
                WCD_DEBUG_ERROR_0("=WCD_BUS_MGR: <Bus has not been activated.>\n");
                rc = E_WCD_BUS_ERROR;
            }
        }

        if (rc == E_WCD_SUCCESS)
        {
            //if the mask is 0xFF, the value of the register will be overwritten
            //regardless of its value so it is safe to ignore the read
            if (mask != 0xFF)
            {
                //for PGD registers, we can check if its non cachable and set the temp 
                //read to the shadow register if the shadow register was initialized.
                if (((ctrl_bus_intf == E_WCD_CODEC_MAIN) || (ctrl_bus_intf == E_WCD_AHB)) && is_shadow_register_initialized)
                {
                    noncachable = WCD_FALSE;
                    noncachable_index = (uint8)(reg >> 3);//same as reg/8 but faster
                    noncachable_bit = reg & 0x07;//same as reg%8 but faster

                    if (noncachable_registers[noncachable_index] & (0x1 << noncachable_bit))
                    {
                        noncachable = WCD_TRUE;
                    }
                }
                else //if (ctrl_bus_intf == E_WCD_INTF)
                {
                    noncachable = WCD_TRUE;
                }

                //if it is noncachable, we need to actually do a bus read
                if (noncachable == WCD_TRUE)
                {
                    WCD_DEBUG_WARN_1("=WCD_BUS_MGR: <Register 0x%x is noncachable. Actually performing register read.>\n", reg);

                    //WCD_DEBUG_CRITICAL_0("+WCD_BUS_MGR: read begin.\n");
                    retry_count = 0;
                    do
                    {
                        if (retry_count != 0)
                        {
                            WCD_SLEEP(RETRY_DELAY_MS);
                        }
                        rc = wcd_osal_bus_read(reg, &temp_read, ctrl_bus_intf);
                    } while (retry_count++ < RETRY_READS_AMOUNT && rc != E_WCD_SUCCESS);
                    //WCD_DEBUG_CRITICAL_0("-WCD_BUS_MGR: read end.\n");
                }
                else
                {
                    //WCD_DEBUG_CRITICAL_0("=WCD_BUS_MGR: Memory access begin.\n");
                    temp_read = shadow_register[reg];
                    //WCD_DEBUG_CRITICAL_0("=WCD_BUS_MGR: Memory access end.\n");
                }
            } //mask != 0xFF
            actual_write = (temp_read & ~mask) | (mask &(value << shift));
            WCD_DEBUG_WARN_2("=WCD_BUS_MGR: <wcd_register_write, Register = %x , Value_Read = %x > \n", reg, temp_read);

            //for the codec registers write to the shadow register
            if (((ctrl_bus_intf == E_WCD_CODEC_MAIN) || (ctrl_bus_intf == E_WCD_AHB)) && is_shadow_register_initialized)
            {
                shadow_register[reg] = actual_write;
            }

            //write new value if no error during reading AND the register is different
            if (rc == E_WCD_SUCCESS)
            {
                if (noncachable == WCD_TRUE || actual_write != temp_read)
                {
                    is_flush_required = WCD_TRUE;

                    retry_count = 0;
                    do
                    {
                        if (retry_count != 0)
                        {
                            WCD_SLEEP(RETRY_DELAY_MS);
                        }
                        rc = wcd_osal_bus_write(reg, actual_write, ctrl_bus_intf, force_flush_write);
                        WCD_DEBUG_WARN_1("=WCD_BUS_MGR: <Register 0x%x is noncachable. Actually performing register write.>\n", reg);
                    } while (retry_count++ < RETRY_WRITES_AMOUNT && rc != E_WCD_SUCCESS);
                }
            }
        }//end of mutex wait verification & activate count verification

        //need to release mutex if it was successfully held
        if (mutex_rc == E_WCD_SUCCESS)
        {
            WCD_RELEASE_MUTEX(context->gh_bus_mutex);
        }
    }

  WCD_DEBUG_WARN_1("-WCD_BUS_MGR: <Register write. RC:%d.>\n", rc);

    return rc;
}

wcd_result wcd_register_write_sync(wcd_ctrl_bus_interface ctrl_bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 value)
{
    wcd_result rc = E_WCD_SUCCESS;
  // WCD_DEBUG_WARN_0("+WCD_BUS_MGR: <Register write sync (blocking)> \n");

    force_flush_write = WCD_TRUE;

    rc = wcd_register_write(ctrl_bus_intf, reg, mask, shift, value);

    force_flush_write = WCD_FALSE;

    is_flush_required = WCD_FALSE;

  // WCD_DEBUG_WARN_1("-WCD_BUS_MGR: <Register write sync (blocking), rc=(%d)> \n", rc);

    return rc;
}

wcd_result wcd_bus_flush(wcd_ctrl_bus_interface ctrl_bus_intf)
{
    wcd_result rc = E_WCD_BUS_ERROR;
    wcd_result mutex_rc = E_WCD_OS_CALL_ERROR;
    int32 wait_result = WCD_WAIT_DONE;
    uint8 retry_count = 0;

    WCD_DEBUG_WARN_0("+WCD_BUS_MGR: <Register flush.>\n");

    if (context)
    {
        wait_result = WCD_WAIT_FOR_MUTEX(context->gh_bus_mutex, WCD_INFINITE);
        if (wait_result == WCD_WAIT_DONE)
        {
            mutex_rc = E_WCD_SUCCESS;
        }

        if (mutex_rc == E_WCD_SUCCESS)
        {
            //initially mark RC as success, assuming activate count is good
            rc = E_WCD_SUCCESS;

            if (context->bus_activate_count == 0)
            {
                WCD_DEBUG_ERROR_0("=WCD_BUS_MGR: <Bus has not been activated.>\n");
                rc = E_WCD_BUS_ERROR;
            }
        }

        if (rc == E_WCD_SUCCESS)
        {
            if (is_flush_required == WCD_TRUE)
            {
                force_flush_write = WCD_TRUE;

                retry_count = 0;
                do
                {
                    if (retry_count != 0)
                    {
                        WCD_SLEEP(RETRY_DELAY_MS);
                    }
                    rc = wcd_osal_bus_write(WCD_FLUSH_REGISTER, 0, ctrl_bus_intf, force_flush_write);
                } while (retry_count++ < RETRY_WRITES_AMOUNT && rc != E_WCD_SUCCESS);

                force_flush_write = WCD_FALSE;

                is_flush_required = WCD_FALSE;

            }//end if flush required
        }

        //need to release mutex if it was successfully held
        if (mutex_rc == E_WCD_SUCCESS)
        {
            WCD_RELEASE_MUTEX(context->gh_bus_mutex);
        }
    }

    WCD_DEBUG_WARN_1("-WCD_BUS_MGR: <Register flush. RC:%d.>\n", rc);

    return rc;
}

/**
* \brief Register read
*
*/
wcd_result wcd_register_read(wcd_ctrl_bus_interface ctrl_bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 *value)
{
    wcd_result rc = E_WCD_BUS_ERROR;
    wcd_result mutex_rc = E_WCD_OS_CALL_ERROR;

    int32 wait_result = 0;
    uint8 temp_read = 0;

    uint8 noncachable_index = 0;
    uint8 noncachable_bit = 0;

    uint8 retry_count = 0;

    wcd_bool noncachable = WCD_TRUE;

  WCD_DEBUG_WARN_0("+WCD_BUS_MGR: <Register read.>\n");
  WCD_DEBUG_WARN_2("=WCD_BUS_MGR: <Register read. Intf: %d reg %u.>\n", ctrl_bus_intf, reg);

    if (context)
    {
        wait_result = WCD_WAIT_FOR_MUTEX(context->gh_bus_mutex, WCD_INFINITE);
        if (WCD_WAIT_DONE == wait_result)
        {
            mutex_rc = E_WCD_SUCCESS;
        }

        if (mutex_rc == E_WCD_SUCCESS)
        {
            //initially mark RC as success, assuming activate count is good
            rc = E_WCD_SUCCESS;

            if (context->bus_activate_count == 0)
            {
                WCD_DEBUG_ERROR_0("=WCD_BUS_MGR: <Bus has not been activated.>\n");
                rc = E_WCD_BUS_ERROR;
            }
        }

        if (E_WCD_SUCCESS == rc)
        {
            //if the shadow_register was not initialized then we just need to blindly read
            //the registers
            if (shadow_register != NULL && is_shadow_register_initialized)
            {
                noncachable = WCD_FALSE;
                if ((ctrl_bus_intf == E_WCD_CODEC_MAIN) || (ctrl_bus_intf == E_WCD_AHB) || (ctrl_bus_intf == E_WCD_INTERRUPT))
                {
                    noncachable_index = (uint8)(reg >> 3);
                    noncachable_bit = reg & 0x07;

                    if (noncachable_registers[noncachable_index] & (0x1 << noncachable_bit))
                    {
                        noncachable = WCD_TRUE;
                    }

                    //WCD_DEBUG_CRITICAL_0("+WCD_BUS_MGR: Memory access begin.\n");
                    temp_read = shadow_register[reg];
                    //WCD_DEBUG_CRITICAL_0("-WCD_BUS_MGR: Memory access end.\n");
                }
            }

            if (noncachable == WCD_TRUE)
            {
                //read the current status of the register to make sure we don't modify 
                //bits we are not interested
                retry_count = 0;
                do
                {
                    if (retry_count != 0)
                    {
                        WCD_SLEEP(RETRY_DELAY_MS);
                    }
                    rc = wcd_osal_bus_read(reg, &temp_read, ctrl_bus_intf);
                } while (retry_count++ < RETRY_READS_AMOUNT && rc != E_WCD_SUCCESS);
            }
            *value = (temp_read & mask) >> shift;
        }

        if (mutex_rc == E_WCD_SUCCESS)
        {
            WCD_RELEASE_MUTEX(context->gh_bus_mutex);
        }
    }

  WCD_DEBUG_WARN_3("=WCD_BUS_MGR: <wcd_register_read_actual, Register = %x , Mask = %x , Read_value = %x > \n ", reg, mask, *value);
  WCD_DEBUG_WARN_2("=WCD_BUS_MGR: <Register read, Intf=(%d), rc=(%d)> \n", ctrl_bus_intf, rc);

    return rc;
}

wcd_result wcd_bus_get_logical_address(uint8 *logical_addr_pgd, uint8 *logical_addr_intf)
{
    wcd_result rc = E_WCD_INVALID_HANDLE;

    if (context)
    {
        rc = wcd_osal_bus_get_logical_address(logical_addr_pgd, logical_addr_intf);
    }

    return rc;
}

wcd_result wcd_bus_init_shadow_register(wcd_codec_info codec_info)
{
    wcd_result return_code = E_WCD_SUCCESS;
    return_code = E_WCD_INITIALIZATION_ERROR;
  do
    {
        //initialize cachable registers
        if (shadow_register == NULL)
        {
      break;
        }
        return_code = wcd_initialize_shadow_register(shadow_register, codec_info);

        if (return_code == E_WCD_SUCCESS)
        {
            noncachable_registers = &(shadow_register[WCD_NUMBER_OF_REGISTERS]);
            return_code = wcd_initialize_noncachable_register(noncachable_registers, codec_info);
        }

    is_shadow_register_initialized = WCD_TRUE;
  }while(WCD_G_FALSE_BOOLEAN);

    return return_code;
}

/**
* \brief Activates the bus for any bus reads/writes
*
*/
wcd_result wcd_activate_bus(void)
{
    wcd_result rc = E_WCD_OS_CALL_ERROR;
    int32 wait_result = WCD_WAIT_DONE;

    if (context)
    {
        wait_result = WCD_WAIT_FOR_MUTEX(context->gh_bus_mutex, WCD_INFINITE);
        if (wait_result == WCD_WAIT_DONE)
        {
            rc = E_WCD_SUCCESS;
            if (context->bus_activate_count == 0)
            {
                WCD_DEBUG_INFO_0("=WCD_BUS_MGR: <Activating bus.>\n");
                rc = wcd_osal_bus_activate();
            }

            if (rc == E_WCD_SUCCESS)
            {
                context->bus_activate_count++;
            }

            WCD_RELEASE_MUTEX(context->gh_bus_mutex);
        }
    }

    return rc;
}

/**
* \brief Deactivates the bus preventing any bus reads/writes
*
*/
wcd_result wcd_deactivate_bus(void)
{
    wcd_result rc = E_WCD_OS_CALL_ERROR;
    int32 wait_result = WCD_WAIT_DONE;

    if (context)
    {
        wait_result = WCD_WAIT_FOR_MUTEX(context->gh_bus_mutex, WCD_INFINITE);
        if (wait_result == WCD_WAIT_DONE)
        {
            rc = E_WCD_SUCCESS;
            if (context->bus_activate_count > 0)
            {
                context->bus_activate_count--;
            }

            if (context->bus_activate_count == 0)
            {
                WCD_DEBUG_INFO_0("=WCD_BUS_MGR: <Deactivating bus.>\n");
                rc = wcd_osal_bus_deactivate();
            }

            WCD_RELEASE_MUTEX(context->gh_bus_mutex);
        }
    }

    return rc;
}


/**
* \brief Dumps the writable registers current values to the log.
*
*/
wcd_result wcd_dump_registers(void)
{
    return wcd_dump_target_registers();
}

