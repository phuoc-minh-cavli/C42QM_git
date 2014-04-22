/*! \file
*
*  \brief  mpp_app.cpp ----File contains the implementation of the public APIs for XO resource type.
*  \details Implementation file for XO resource type.
*
*  &copy;  Copyright (c) 2013-2016, 2019 Qualcomm Technologies, Inc. All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/xo/src/pm_xo.c#3 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   rh      File created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_xo_core.h"
#include "pm_xo_driver.h"
#include "hw_module_type.h"

#define CLK_DIST_BASE_ADDRESS   0x5900
#define CLK_SLAVE_ID           0
#define CLK_DIST_SPARE_REG_1   (CLK_DIST_BASE_ADDRESS + 0x51)
#define CLK_DIST_SPARE_REG_2   (CLK_DIST_BASE_ADDRESS + 0x52)

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_dev_xo_core_set_power_mode(uint8 pmic_chip, int internalResourceIndex, pm_xo_core_power_mode_type  mode)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_xo_core_data_type *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if  ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        errFlag = PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }
    else if( mode > PM_XO_CORE_PWR_MODE__HPM )
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) +
              xo_ptr->xo_core_register->mode_ctl1;

        errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x03, (pm_register_data_type)(mode), 0);   // 1:0
    }

    /* Let the user know if we were successful or not */
    return errFlag;
}

pm_err_flag_type pm_dev_mega_xo_set_xo_trim(uint8 pmic_chip, int internalResourceIndex, uint8 trim_value)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_err_flag_type      err       = PM_ERR_FLAG__SUCCESS;
    pm_xo_core_data_type  *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if  ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }

    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) + xo_ptr->xo_core_register->xo_adj;

    if(trim_value > (xo_ptr->xo_core_common->trim_max))
    {
        /* Round the value to the boundary */
        trim_value = xo_ptr->xo_core_common->trim_max;

        /* Set error flag */
        err = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x3F, (pm_register_data_type)trim_value, 0 ); // 5:0

    if(err == PM_ERR_FLAG__SUCCESS)
    {
        return errFlag;
    }
    else
    {
        return err;
    }
}

pm_err_flag_type pm_dev_mega_xo_get_xo_trim(uint8 pmic_chip, int internalResourceIndex, uint8*  trim_value)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_xo_core_data_type  *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if  ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }
    if(NULL == trim_value)
    {
        return PM_ERR_FLAG__INVALID_POINTER ;
    }

    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) + xo_ptr->xo_core_register->xo_adj;

    errFlag = pm_comm_read_byte(xo_ptr->comm_ptr->slave_id, reg, &data, 0);

    if (errFlag == PM_ERR_FLAG__SUCCESS)
    {
        *trim_value = (uint8)(data & 0x3F); // 5:0
    }

    return errFlag;
}

pm_err_flag_type pm_dev_mega_xo_set_xo_core_force_on(uint8 pmic_chip, int internalResourceIndex,boolean  xo_core_enable)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_xo_core_data_type  *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }

    pm_register_data_type    data = 0;
    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) + xo_ptr->xo_core_register->en_ctl;

    if(TRUE == xo_core_enable)
    {
        data = 0xFF;
    }
    else
    {
        data = 0;
    }

    errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x80, data, 0 ); // 7

    return errFlag;
}

pm_err_flag_type pm_xo_core_set_xo_comp(uint8 pmic_chip, int8 cap_value)
{
    pm_err_flag_type      	  errFlag                = PM_ERR_FLAG__SUCCESS;
    pm_err_flag_type      	  err                    = PM_ERR_FLAG__SUCCESS;
    pm_xo_core_data_type 	 *xo_ptr                 = pm_xo_core_get_data(pmic_chip);
	pm_register_address_type  reg                    = 0x0;
	pm_register_data_type     periph_type            = 0x0;
	pm_register_data_type     periph_subtype         = 0x0;
    pm_register_data_type     data                   = 0x0;
    uint8                     internalResourceIndex  = 0x0; // hard coded since this couldn't be expected from caller.

	if (xo_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    if(cap_value > (xo_ptr->xo_core_common->cap_val_max))
    {
       /* Round the value to the boundary */
       cap_value = xo_ptr->xo_core_common->cap_val_max;
       
       /* Set error flag */
       err = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

	reg      = xo_ptr->xo_core_register->base_address + xo_ptr->xo_core_register->peripheral_type;
    errFlag  = pm_comm_read_byte(xo_ptr->comm_ptr->slave_id, reg,  &periph_type, 0);
	reg      = xo_ptr->xo_core_register->base_address + xo_ptr->xo_core_register->peripheral_subtype;
    errFlag |= pm_comm_read_byte(xo_ptr->comm_ptr->slave_id, reg, &periph_subtype, 0);
   
	if ((periph_type == PM_HW_MODULE_CLOCK) && (periph_subtype == PM_HW_MODULE_CLOCK_XO_CORE))
	{
        /* Get the XO_ADJ_FINE Reg address */
		reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset * internalResourceIndex) + xo_ptr->xo_core_register->xo_adj;
		
		/* Read the current XO_ADJ_FINE reg */
		errFlag |= pm_comm_read_byte(xo_ptr->comm_ptr->slave_id, reg, &data, 0);

		/* Copy value of  XO_ADJ_FINE to CLK_DIST_SPARE1 */
		errFlag |= pm_comm_write_byte(CLK_SLAVE_ID, CLK_DIST_SPARE_REG_1, data, 0);

		/* Calculate (XO_ADJ_FINE+trim_value) & Copy value to CLK_DIST_SPARE2 */
		errFlag |= pm_comm_write_byte(CLK_SLAVE_ID, CLK_DIST_SPARE_REG_2, data + cap_value, 0);
	}

	else
    {
	    reg = xo_ptr->xo_core_register->base_address + xo_ptr->xo_core_register->xo_comp;
	    errFlag |= pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x1F, (pm_register_data_type)cap_value, 0 ); // 4:0
	}
    
    if(err == PM_ERR_FLAG__SUCCESS)
    {
        return errFlag;
    }
    else
    {
        return err;
    }
}

pm_err_flag_type pmapp_xo_core_set_xo_dcc_adj(uint8 dcc_adjust)
{
    return pm_xo_core_set_xo_dcc_adj(0, dcc_adjust);
}

pm_err_flag_type pm_xo_core_set_xo_dcc_adj(uint8 pmic_chip, uint8 dcc_adjust)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_err_flag_type      err       = PM_ERR_FLAG__SUCCESS;
    pm_xo_core_data_type *xo_ptr = pm_xo_core_get_data(pmic_chip);
    uint8  reg_mask = 0;

    if (xo_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if(dcc_adjust > xo_ptr->xo_core_common->dcc_adjust_max)
    {
       /* Round the value to the boundary */
       dcc_adjust = xo_ptr->xo_core_common->dcc_adjust_max;
       
       /* Set error flag */
       err = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    
    reg_mask = xo_ptr->xo_core_common->dcc_adjust_max; 
    
    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + xo_ptr->xo_core_register->xo_dcc_adj;
    errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, reg_mask, (pm_register_data_type)dcc_adjust, 0 ); 

    if(err == PM_ERR_FLAG__SUCCESS)
    {
        return errFlag;
    }
    else
    {
        return err;
    }

}

pm_err_flag_type pmapp_xo_core_get_xo_dcc_adj(uint8* dcc_adjust)
{
    return pm_xo_core_get_xo_dcc_adj(0, dcc_adjust);
}

pm_err_flag_type pm_xo_core_get_xo_dcc_adj(uint8 pmic_chip, uint8* dcc_adjust)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_xo_core_data_type *xo_ptr = pm_xo_core_get_data(pmic_chip);
    uint8  reg_mask = 0;
	
    if (xo_ptr == NULL) 
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if(NULL == dcc_adjust)
    {
        return PM_ERR_FLAG__INVALID_POINTER ;
    }
	
    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + xo_ptr->xo_core_register->xo_dcc_adj;
	
    errFlag = pm_comm_read_byte(xo_ptr->comm_ptr->slave_id, reg, &data, 0); 

    if (errFlag == PM_ERR_FLAG__SUCCESS)
    {
        reg_mask = xo_ptr->xo_core_common->dcc_adjust_max; 
        *dcc_adjust = (uint8)(data & reg_mask);
    }

    return errFlag;

}

/*===========================================================================

                        DEPRECATED FUNCTIONS

===========================================================================*/
pm_err_flag_type pm_xo_core_set_power_mode(int externalResourceIndex, pm_xo_core_power_mode_type mode)
{
    return pm_dev_xo_core_set_power_mode(0, externalResourceIndex, mode);
}
pm_err_flag_type pm_mega_xo_set_xo_trim(int externalResourceIndex, uint8 trim_value)
{
    return pm_dev_mega_xo_set_xo_trim(0, externalResourceIndex, trim_value);
}
pm_err_flag_type pm_xo_core_get_xo_trim(int externalResourceIndex, uint8* trim_value)
{
    return pm_dev_mega_xo_get_xo_trim(0, externalResourceIndex, trim_value);
}
pm_err_flag_type pm_xo_core_set_xo_core_force_on(int externalResourceIndex, boolean xo_core_enable)
{
    return pm_dev_mega_xo_set_xo_core_force_on(0, externalResourceIndex, xo_core_enable);
}
