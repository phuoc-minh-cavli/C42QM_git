/**
@file anc_manager_target.c
@brief 
Implementation and internal functions of WCD ANC Manager target specific file
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/anc_manager_target.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/31/13   KNM		Adopted common code specific logging message related changes.
03/01/13   KNM	    Initial revision
===============================================================================*/

#include "anc_manager_target.h"
#include "bus_manager.h"
#include "wcd_gen_elements.h"
#include "target_defs.h"

#ifdef SPL_LOG_SUPPORT
	#include "anc_manager_target.tmh"
#endif

/******************************************************************************
 **************                ANC ELEMENT MAPPINGS             ***************
 *****************************************************************************/

// ADC WCD elements
wcd_element* anc_ADC_elements[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4, &wcd_ADC_5 };

// DMIC WCD elements
wcd_element* anc_DMIC_elements[] = { &wcd_DMIC_1, &wcd_DMIC_2, &wcd_DMIC_3, &wcd_DMIC_4 };


/**
 * \brief Maps an ANC input device to the codec source element.
 */
wcd_result wcd_map_anc_source(wcd_adie_codec_db_anc_input_device anc_source,
                              wcd_element** source_element)
{
  *source_element = NULL;

  switch( anc_source )
  {
    case E_ANC_DEV_ADC1:
      *source_element = anc_ADC_elements[0];
      break;
    case E_ANC_DEV_ADC2:
      *source_element = anc_ADC_elements[1];
      break;
    case E_ANC_DEV_ADC3:
      *source_element = anc_ADC_elements[2];
      break;
    case E_ANC_DEV_ADC4:
      *source_element = anc_ADC_elements[3];
      break;
    case E_ANC_DEV_ADC5:
      *source_element = anc_ADC_elements[4];
	  break;
    case E_ANC_DEV_DMIC1:
      *source_element = anc_DMIC_elements[0];
      break;
    case E_ANC_DEV_DMIC2:
      *source_element = anc_DMIC_elements[1];
      break;
    case E_ANC_DEV_DMIC3:
      *source_element = anc_DMIC_elements[2];
      break;
    case E_ANC_DEV_DMIC4:
      *source_element = anc_DMIC_elements[3];
      break;
    default:
      break;
  }

  if (*source_element == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error mapping ANC input device ENUM to WCD element.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  return E_WCD_SUCCESS;
}


/**
 * \brief Connect microphones to ANC connections.
 */
wcd_result wcd_set_anc_connections(wcd_adie_codec_db_anc_config *anc_config)
{
  uint16 reg = WCD_ANC_CONN_R;
  uint8  mask = WCD_ANC_CONN_M;
  uint8  shift = WCD_ANC_CONN_S;
  uint8  value = 0;
  uint8  temp = 0;

  //0x1 if the anc config is a DMIC, 0x0 if anc config is ADC
  uint8  anc1_dmic = 0x0;
  uint8  anc2_dmic = 0x0;

  wcd_result return_code = E_WCD_SUCCESS;

  if (anc_config[0].default_enable != WCD_FALSE)
  {
    temp = (uint8)(anc_config[0].input_device);

    value |= temp << WCD_ANC_CONN_ANC1_S;

    if (anc_config[0].input_device >= E_ANC_DEV_DMIC1 && anc_config[0].input_device <= E_ANC_DEV_DMIC4)
    {
      anc1_dmic = 0x1;
    }
  }

  if (anc_config[1].default_enable != WCD_FALSE)
  {
    temp = (uint8)(anc_config[1].input_device);

    value |= temp << WCD_ANC_CONN_ANC2_S;

    if (anc_config[1].input_device >= E_ANC_DEV_DMIC1 && anc_config[1].input_device <= E_ANC_DEV_DMIC4)
    {
      anc2_dmic = 0x1;
    }
  }

  /* ADC/DMIC input selection for ANC in CDC_CONN_ANC_B1_CTL */
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);

  /* CDC_ANC1_B1_CTL[1] set to anc1_dmic & CDC_ANC2_B1_CTL[1] set to anc2_dmic */
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, WCD_ANC_ADC_DMIC_SEL_R1, WCD_ANC_ADC_DMIC_SEL_M, WCD_ANC_ADC_DMIC_SEL_S, anc1_dmic);
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, WCD_ANC_ADC_DMIC_SEL_R2, WCD_ANC_ADC_DMIC_SEL_M, WCD_ANC_ADC_DMIC_SEL_S, anc2_dmic);

  return return_code;
}

/**
 * \brief Disconnects microphone from ANC channels
 */
wcd_result wcd_disable_anc_connections(void)
{
  uint16 reg = WCD_ANC_CONN_R;
  uint8  mask = WCD_ANC_CONN_M;
  uint8  shift = WCD_ANC_CONN_S;
  uint8  value = 0;

  return wcd_register_write(E_WCD_CODEC_MAIN, reg,mask,shift,value);
}

/**
 * \brief Check if ANC feature is supported or not
 */
wcd_bool wcd_is_anc_supported(void)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_bool rc = WCD_TRUE;
  wcd_codec_info codec_info;

  return_code = wcd_get_codec_info(&codec_info);
  if (return_code != E_WCD_SUCCESS)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting codec info.>\n");
	rc = WCD_FALSE;
  }

  if(codec_info.codec == E_WCD_WCD9302)
  {
    rc = WCD_FALSE;
  }
  
  return rc;    
}
