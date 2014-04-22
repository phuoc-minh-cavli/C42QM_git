/**
  @file qurt_clade.h 
  @brief  Prototypes of clade interface API.  
	 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_clade2_get
  Reads the value of clade2 register
 
  @param[in] offset offset from the QURTK_clade2_cfg_base          
 
  @param[in] *value  register value read from the offset.
 
  @return
  QURT_EOK - successfully read the value from the register at offset
  QURT_EINVALID - offset passed is incorrect
   
  @dependencies
  None.
 */
int qurt_clade2_get(unsigned short offset, unsigned int *value);
 
/**@ingroup func_qurt_clade2_set
  Sets the PMU register.\n
  only PMU_SEL register can be set
  
  @param[in] offset offset from the QURTK_clade2_cfg_base          
 
  @param[in] value  value to be set at offset.  
 
  @return
   QURT_EOK - successfully set the value at offset
   QURT_ENOTALLOWED - set operation performed at an offset other than CLADE2_PMU_SELECTION_REG

  @dependencies
  None.
 */
int qurt_clade2_set(unsigned short offset, unsigned int value);


