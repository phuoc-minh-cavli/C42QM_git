/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef __GPIO_TOGGLE_H__
#define __GPIO_TOGGLE_H__

typedef enum 
{
    REQ_GPIO_NUM   = 0x626d756e,  // 'numb' in little endian
    REQ_GPIO_STATE = 0x74617473,  // 'stat' in little endian
    REQ_GPIO_MODE  = 0x65646f6d,  // 'mode' in little endian
} gpio_toggle_keys;

typedef enum
{
    GPIO_TOGGLE_APPLY       = 0x00, //0b00000000
    GPIO_TOGGLE_VDD_MIN     = 0x01, //0b00000001
    GPIO_TOGGLE_XO_SHUTDOWN = 0x02, //0b00000010
    GPIO_TOGGLE_HALT        = 0x04, //0b00000100
} gpio_toggle_mode_mask;

typedef enum
{
    GPIO_TOGGLE_STATE_LOW  = 0,
    GPIO_TOGGLE_STATE_HIGH = 1,
} gpio_toggle_states;

#endif
