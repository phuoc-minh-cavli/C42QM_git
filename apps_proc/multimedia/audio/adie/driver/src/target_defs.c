/**
@file target_defs.c
@brief
Implementation and internal functions of WCD target definitions
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/target_defs.c#6 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/13/13   AG      Added new speaker PA element type
09/12/13   KNM     Resolved compiler error.
09/11/13   AG      Set bit to enable micbias pulldown
                   Earout CnP changes
                   Codec Sidetone connect second IIR filter
09/01/13   KNM     Set default GAIN_SOURCE_SEL of all RX path as REGISTER
                   in reset_codec.
08/06/13   KNM     Moved shadow register initialization to wcd_reset_codec and
                   added DMIC leakage mitigation by switching DMIC TLMM pins.
07/25/13   KNM     Added a generic function to determine connection direction.
07/23/13   KNM     Added ANC FB Tune1 & Tune2 routing in Tx path and implemented
                   V5V6 specific changes for dynamic buck voltage switching.
05/31/13   KNM     Added support for wcd9302 codec tier and adopted common code
                   and multichannel sidetone usecase related changes.
03/21/13   KNM     Change LDO voltage from 3V to 2.35V.
03/01/13   KNM     Initial revision.
===============================================================================*/

#include "wcd_utility.h"
#include "target_defs.h"
#include "wcd_gen_elements.h"
#include "wcd_gen_id_defines.h"
#include "bus_manager.h"
#include "init_manager.h"
#include "wcd_custom_functions.h"
#include "wcd_types.h"

//#ifdef WCD_MBHC_ENABLE
  #include "audio_resource_manager.h"
//#endif

#ifdef SPL_LOG_SUPPORT
	#include "target_defs.tmh"
#endif

/******************************************************************************
 **************               FORWARD DECLERATION               ***************
 *****************************************************************************/
wcd_codec_info g_wcd_codec_info = {E_WCD_CODEC_32B_ANCHOR, E_WCD_CODEC_VERSION_32B_ANCHOR};

static wcd_vdd_buck_voltage wcd_codec_buck_voltage = E_WCD_VDD_BUCK_2P15V;


/******************************************************************************
 **************               SOURCE/SINK MAPPING               ***************
 *****************************************************************************/

//Integers for which RX and TX ports are currently used
uint32  used_rx_bus_ports = 0;
uint32  used_tx_bus_ports = 0;

// Number of RX and TX ports on the codec
#define WCD_MAX_RX_PORTS      5
#define WCD_MAX_TX_PORTS      5
#define WCD_MAX_I2S_RX_PORTS  2
#define WCD_MAX_I2S_TX_PORTS  2
#define WCD_I2S_TX_OFFSET     0

static uint32 wcd_max_adcs;
static uint32 wcd_max_dmics;

// Number of ADCs on wcd9306 codec
/* Note: Excluding MBADC physically only 4 ADCs are available
   however IN2, IN3 inputs are muxed and mapped to same ADC#2/3
   and swi still shows as if 2 full paths are available for ADC2 and ADC3.
   ADC3 takes priority.
*/
#define WCD_MAX_ADCS_WCD9306    5

// Number of DMICs on wcd9306 codec
#define WCD_MAX_DMICS_WCD9306   4

// Number of ADCs on wcd9302 codec
/* Note: Though ADC#5 is present in codec but connection to decimator
         has been qfused, so effective only 4 ADCs are available */
#define WCD_MAX_ADCS_WCD9302    4

// Number of DMICs on wcd9302 codec
#define WCD_MAX_DMICS_WCD9302   2

// Number of LINEOUTs on the codec
#define WCD_MAX_LINEOUTS        2

//number of ANC CHANNELs on the codec
#define WCD_MAX_ANC_CHANNELS  2

#define CATEGORY_MASK                   0xFF0000
#define DYNAMIC_BUS_INPUT_ID            0

//Source Masks for the different categories of sources
#define SOURCE_AUDIO_INPUT_MASK         0x10000
#define SOURCE_DMIC_INPUT_MASK          0x20000
#define SOURCE_SLIMBUS_RX_INPUT_MASK    0x40000
#define SOURCE_I2S_RX_INPUT_MASK        0x80000
#define SOURCE_REUSE_BUS_PORT_MASK      0x100000
#define SOURCE_ANC_CHANNEL_MASK         0x200000

//Sink Masks for the different categories of sinks
#define SINK_SPECIAL_SINK_MASK          0x10000
#define SINK_LINE_MASK                  0x20000
#define SINK_SLIMBUS_TX_MASK            0x40000
#define SINK_I2S_TX_INPUT_MASK          0x80000

//Sink IDs for special sinks
#define SINK_EAROUT_ID                  0x1
#define SINK_ULTRASOUND_EAROUT_ID       0x11
#define SINK_SPEAKER_ID                 0x2
#define SINK_HPHL_ID                    0x3
#define SINK_HPHR_ID                    0x4

//Mapping for which ADCs can connect to which SBTX port
uint32  wcd9306_adc_1_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_adc_2_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_adc_3_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_adc_4_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_adc_5_sbtx_map[] = { 1, 2, 3, 4, 0 };

uint32  wcd9302_adc_1_sbtx_map[] = { 1, 2, 5, 0 };
uint32  wcd9302_adc_2_sbtx_map[] = { 1, 2, 5, 0 };
uint32  wcd9302_adc_3_sbtx_map[] = { 1, 2, 5, 0 };
uint32  wcd9302_adc_4_sbtx_map[] = { 1, 2, 5, 0 };
uint32  wcd9302_adc_5_sbtx_map[] = { 0 };

uint32  **adc_sbtx_map = NULL;

uint32* wcd9306_adc_sbtx_map[] =  { wcd9306_adc_1_sbtx_map,
                                    wcd9306_adc_2_sbtx_map,
                                    wcd9306_adc_3_sbtx_map,
                                    wcd9306_adc_4_sbtx_map,
                                    wcd9306_adc_5_sbtx_map
                                  };

uint32* wcd9302_adc_sbtx_map[] =  { wcd9302_adc_1_sbtx_map,
                                    wcd9302_adc_2_sbtx_map,
                                    wcd9302_adc_3_sbtx_map,
                                    wcd9302_adc_4_sbtx_map,
                                    wcd9302_adc_5_sbtx_map
                          };

//Mapping for which DMICs can connect to which SBTX port
uint32  wcd9306_dmic_1_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_dmic_2_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_dmic_3_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };
uint32  wcd9306_dmic_4_sbtx_map[] = { 1, 2, 3, 4, 5, 0 };

uint32  wcd9302_dmic_1_sbtx_map[] = { 1, 2, 5, 0 };
uint32  wcd9302_dmic_2_sbtx_map[] = { 1, 2, 5, 0 };
uint32  wcd9302_dmic_3_sbtx_map[] = { 0 };
uint32  wcd9302_dmic_4_sbtx_map[] = { 0 };

uint32* wcd9306_dmic_sbtx_map[] = { wcd9306_dmic_1_sbtx_map,
                                    wcd9306_dmic_2_sbtx_map,
                                    wcd9306_dmic_3_sbtx_map,
                                    wcd9306_dmic_4_sbtx_map
                                  };

uint32* wcd9302_dmic_sbtx_map[] = { wcd9302_dmic_1_sbtx_map,
                                    wcd9302_dmic_2_sbtx_map,
                                    wcd9302_dmic_3_sbtx_map,
                                    wcd9302_dmic_4_sbtx_map
                                  };

uint32 **dmic_sbtx_map = NULL;

//Mapping for which ANC Channel can connect to which SBTX port
uint32  anc_channel_1_sbtx_map[] = { 1, 2, 3, 0 };
uint32  anc_channel_2_sbtx_map[] = { 1, 2, 4, 0 };

uint32* anc_channel_sbtx_map[] =  { anc_channel_1_sbtx_map,
                                    anc_channel_2_sbtx_map
                                  };


// SBRX WCD elements
wcd_element* SBRX_elements[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5};

// SBTX WCD elements
wcd_element* SBTX_elements[] = { &wcd_SBTX_1, &wcd_SBTX_2, &wcd_SBTX_3, &wcd_SBTX_4, &wcd_SBTX_5};

// ADC WCD elements
wcd_element** ADC_elements = NULL;
wcd_element* wcd9306_ADC_elements[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4, &wcd_ADC_5};
wcd_element* wcd9302_ADC_elements[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4};

// DMIC WCD elements
wcd_element** DMIC_elements = NULL;
wcd_element* wcd9306_DMIC_elements[] = { &wcd_DMIC_1, &wcd_DMIC_2, &wcd_DMIC_3, &wcd_DMIC_4};
wcd_element* wcd9302_DMIC_elements[] = { &wcd_DMIC_1, &wcd_DMIC_2};

// ANC WCD elements
wcd_element* ANC_elements[] = { &wcd_ANC_CHANNEL_1, &wcd_ANC_CHANNEL_2 };

// RDAC WCD elements
wcd_element* EAROUT_element = &wcd_EAROUT;
wcd_element* HPHL_element = &wcd_HPHL;
wcd_element* HPHR_element = &wcd_HPHR;
wcd_element* SPEAKER_element = &wcd_SPEAKER;
wcd_element* LO_elements[] = { &wcd_LO1_L_P, &wcd_LO2_R_M };

/** \brief Validates the source and maps the source to the appropriate element.
 *
 */
wcd_result wcd_map_source_to_element(wcd_usecase* usecase, wcd_source* source, wcd_element** p_element_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32  source_category = *source & CATEGORY_MASK;
  uint32  source_id       = *source & (~CATEGORY_MASK);

  uint32  bus_port_iterator = 0;
  uint32  maximum_ports = WCD_MAX_RX_PORTS;
  uint32  used_bus_port = 0;

  wcd_connection_info* previous_connection = NULL;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Map source to element.>\n");

  do
  {
    if (source_category == SOURCE_SLIMBUS_RX_INPUT_MASK || source_category == SOURCE_I2S_RX_INPUT_MASK)
    {
      if ( source_id > WCD_MAX_RX_PORTS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Source is beyond available number of bus ports on codec.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      if (source_category == SOURCE_I2S_RX_INPUT_MASK)
      {
        maximum_ports = WCD_MAX_I2S_RX_PORTS;
      }

      if ( source_id == DYNAMIC_BUS_INPUT_ID)
      {
        WCD_DEBUG_INFO_0("=WCD_93XX: <Dynamic bus port. Determining bus port.>\n");

        //loop through number of ports
        while(bus_port_iterator < maximum_ports)
        {
          //if shifting the used ports and it is used...then go on
          if ( (used_rx_bus_ports >> bus_port_iterator) & (0x1) )
          {
            bus_port_iterator++;
            continue;
          }

          //if it is not used we found a bus_port
          break;
        }

        if (bus_port_iterator == maximum_ports)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <No available bus ports.>\n");
          return_code = E_WCD_INVALID_SOURCE;
          break;
        }

        used_bus_port = bus_port_iterator + 1;

        //set the source to the bus port figured out
        *source += used_bus_port;
      }
      else
      {
        used_bus_port = source_id;
      }

      if (used_bus_port < 1 || used_bus_port > maximum_ports)
      {
        WCD_DEBUG_INFO_0("=WCD_93XX: <Invalid bus port determined.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      //mark the port as used
      used_rx_bus_ports = used_rx_bus_ports | (0x1 << (used_bus_port - 1));
      WCD_DEBUG_INFO_1("=WCD_93XX: <Determined busport. SBRX %u.>\n", (unsigned int)used_bus_port);

      //figure out element handle
      *p_element_handle = SBRX_elements[used_bus_port - 1];

    }
    else if (source_category == SOURCE_REUSE_BUS_PORT_MASK)
    {
      //this connection being connected right now has not been added to the usecase's
      //connection list yet
      //so the previous connection created is the tail!
      WCD_DEBUG_INFO_0("=WCD_93XX: <Reusing previous connection's bus port.>\n");

      if (usecase->connection_list == NULL || usecase->connection_list->tail == NULL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Previous connection not found.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      previous_connection = (wcd_connection_info*)usecase->connection_list->tail->data;

      if ( previous_connection == NULL )
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Previous connection not found.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      if (previous_connection->route == NULL ||
          previous_connection->route->elements == NULL ||
          previous_connection->route->elements->head == NULL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Previous connection's route is NULL.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      //we can assume the previous connection's bus port is the head element because it has to be RX!!
      if ( ((wcd_element*)previous_connection->route->elements->head->data)->type != WCD_BUS_PORT_TYPE_ID )
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Previous connection's route's head element is not BUS_PORT type.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      //element handle is the same
      *p_element_handle = (wcd_element*)previous_connection->route->elements->head->data;
    }
    else if (source_category == SOURCE_AUDIO_INPUT_MASK)
    {
      if (source_id > wcd_max_adcs || source_id <= 0)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Source is beyond available number of ADCs on codec.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      //figure out element handle
      *p_element_handle = ADC_elements[source_id - 1];
    }
    else if (source_category == SOURCE_DMIC_INPUT_MASK)
    {
      if (source_id > wcd_max_dmics || source_id <= 0)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Source is beyond available number of DMICs on codec.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      //figure out element handle
      *p_element_handle = DMIC_elements[source_id - 1];
    }
    else if (source_category == SOURCE_ANC_CHANNEL_MASK)
    {
      if (source_id > WCD_MAX_ANC_CHANNELS || source_id <= 0)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Source is beyond available number of ANC Channels on codec.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      *p_element_handle = ANC_elements[source_id - 1];
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Map source to element. RC:%d.>\n", return_code);

  return return_code;
}

/** \brief Validates the sink and maps the sink to the appropriate element.
 *
 */
wcd_result wcd_map_sink_to_element(wcd_source source, wcd_sink* sink,
                                   wcd_connection_type connection_type,
                                   wcd_element** p_element_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32  source_category = source & CATEGORY_MASK;
  uint32  source_id       = source & (~CATEGORY_MASK);

  uint32  sink_category   = *sink & CATEGORY_MASK;
  uint32  sink_id         = *sink & (~CATEGORY_MASK);

  //this is only necessary for I2S
  uint32  minimum_bus_port = 1;

  uint32* available_bus_ports = NULL;

  uint32  bus_port_iterator = 0;
  uint32  maximum_ports = WCD_MAX_TX_PORTS;
  uint32  used_bus_port = 0;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Map sink to element.>\n");

  do
  {
    if (sink_category == SINK_SLIMBUS_TX_MASK || sink_category == SINK_I2S_TX_INPUT_MASK)
    {
      if (connection_type == E_WCD_CONNECTION_DIFFERENTIAL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <TX connections only support DEFAULT connection types.>\n");
        return_code = E_WCD_INVALID_CONNECTION_TYPE;
        break;
      }

      if (sink_category == SINK_I2S_TX_INPUT_MASK)
      {
        //only for i2s
        minimum_bus_port += WCD_I2S_TX_OFFSET;

        if (sink_id != DYNAMIC_BUS_INPUT_ID)
        {
          sink_id = sink_id + WCD_I2S_TX_OFFSET;
        }
      }

      if (source_category == SOURCE_AUDIO_INPUT_MASK)
      {
        if (source_id <= 0 || source_id > wcd_max_adcs)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Source is beyond available number of ADCs on codec.>\n");
          return_code = E_WCD_INVALID_SOURCE;
          break;
        }

        available_bus_ports = adc_sbtx_map[source_id - 1];
      }
      else if (source_category == SOURCE_DMIC_INPUT_MASK)
      {
        if (source_id <= 0 || source_id > wcd_max_dmics)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Source is beyond available number of DMICs on codec.>\n");
          return_code = E_WCD_INVALID_SOURCE;
          break;
        }

        available_bus_ports = dmic_sbtx_map[source_id - 1];
      }

      if (available_bus_ports == NULL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not determine bus ports based on source.>\n");
        return_code = E_WCD_INVALID_SOURCE;
        break;
      }

      if (sink_id == DYNAMIC_BUS_INPUT_ID)
      {
        WCD_DEBUG_INFO_0("=WCD_93XX: <Dynamic bus port. Determining bus port.>\n");

        while(available_bus_ports[bus_port_iterator] != 0)
        {
          //this is for i2s
          if (available_bus_ports[bus_port_iterator] < minimum_bus_port)
          {
            bus_port_iterator++;
            continue;
          }

          //check if the port is currently used
          if ( (0x1 << (available_bus_ports[bus_port_iterator] - 1)) & used_tx_bus_ports )
          {
            bus_port_iterator++;
            continue;
          }

          //found a port
          break;
        }

        if (available_bus_ports[bus_port_iterator] == 0)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <No available bus ports.>\n");
          return_code = E_WCD_INVALID_SINK;
          break;
        }

        used_bus_port = available_bus_ports[bus_port_iterator];

        //set the sink to the bus port figured out
        *sink += used_bus_port;

        //if i2s we need to move it back from the offset
        if (sink_category == SINK_I2S_TX_INPUT_MASK)
        {
          *sink -= WCD_I2S_TX_OFFSET;
        }
      }
      else
      {
        //i2s offset already taken into account
        used_bus_port = sink_id;
      }

      if (used_bus_port < minimum_bus_port || used_bus_port > maximum_ports)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid bus port determined.>\n");
        return_code = E_WCD_INVALID_SINK;
        break;
      }

      //mark the port as used
      used_tx_bus_ports = used_tx_bus_ports | (0x1 << (used_bus_port - 1));
      WCD_DEBUG_INFO_1("=WCD_93XX: <Determined busport. SBTX %u.>\n", (unsigned int)used_bus_port);

      //figure out element handle
      *p_element_handle = SBTX_elements[used_bus_port - 1];

    }
    else if (sink_category == SINK_SPECIAL_SINK_MASK)
    {
      if (connection_type == E_WCD_CONNECTION_DIFFERENTIAL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <EAROUT/HPH/SPEAKER only support DEFAULT connections.>\n");
        return_code = E_WCD_INVALID_CONNECTION_TYPE;
        break;
      }

      //figure out element handle
      if (sink_id == SINK_EAROUT_ID || sink_id == SINK_ULTRASOUND_EAROUT_ID)
      {
        *p_element_handle = EAROUT_element;
      }
      else if (sink_id == SINK_SPEAKER_ID)
      {
        *p_element_handle = SPEAKER_element;
      }
      else if (sink_id == SINK_HPHL_ID)
      {
        *p_element_handle = HPHL_element;
      }
      else if (sink_id == SINK_HPHR_ID)
      {
        *p_element_handle = HPHR_element;
      }
    }
    else if (sink_category == SINK_LINE_MASK)
    {
      if (sink_id > WCD_MAX_LINEOUTS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Sink is beyond available number of LINEOUTs on the codec.>\n");
        return_code = E_WCD_INVALID_SINK;
        break;
      }

      if (connection_type == E_WCD_CONNECTION_DIFFERENTIAL)
      {
        //only LINE1/LINE2 support differential connections
        if (sink_id != 1 && sink_id != 2)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Differential only supported by LINEOUT 1 & LINEOUT 2.>\n");
          return_code = E_WCD_INVALID_CONNECTION_TYPE;
          break;
        }
      }

      //figure out element handle
      *p_element_handle = LO_elements[sink_id - 1];
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Map sink to element. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Validates the sink and maps the sink to the differential element.
 */
wcd_result wcd_map_sink_to_differential_element(wcd_sink sink,
                                   wcd_connection_type connection_type,
                                   wcd_element** p_element_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32  sink_category   = sink & CATEGORY_MASK;
  uint32  sink_id         = sink & (~CATEGORY_MASK);

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Map sink to differential element.>\n");

    if (connection_type != E_WCD_CONNECTION_DIFFERENTIAL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connection type is not differential.>\n");
      return_code = E_WCD_INVALID_CONNECTION_TYPE;
      break;
    }

    if (sink_category != SINK_LINE_MASK)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Differential only supports lineout connections.>\n");
      return_code = E_WCD_INVALID_SINK;
      break;
    }

    if (sink_id == 1)
    {
      *p_element_handle = LO_elements[2 - 1];
    }
    else
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Differential only supports LINEOUT 1 and LINEOUT 2.>\n");
      return_code = E_WCD_INVALID_SINK;
      break;
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Map sink to differential element. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Determines the slimbus port number based on the element passed in
 * Also determines if it is either a RX or TX port
 */
wcd_result wcd_determine_slimbusport_number(wcd_element* bus_port_element,
                                            uint32* bus_port_id,
                                            wcd_slimbus_direction* direction)
{
  uint32 iterator = 0;

  if (bus_port_element == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid handle.>\n");
    return E_WCD_INVALID_HANDLE;
  }

  *bus_port_id = 0;

  for(iterator = 0; iterator < WCD_MAX_RX_PORTS; iterator++)
  {
    if ( SBRX_elements[iterator] == bus_port_element )
    {
      *bus_port_id = iterator + 1;
      *direction = E_WCD_SLIMBUS_RX;
      break;
    }
  }

  for(iterator = 0; iterator < WCD_MAX_TX_PORTS; iterator++)
  {
    if ( SBTX_elements[iterator] == bus_port_element )
    {
      *bus_port_id = iterator + 1;
      *direction = E_WCD_SLIMBUS_TX;
      break;
    }
  }

  if (*bus_port_id == 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to determine bus port number. Probably a bad element.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  return E_WCD_SUCCESS;
}

/**
 * \brief Marks a TX/RX slimbus port as freed and allows other connections
 */
wcd_result wcd_free_slimbusport(wcd_element* bus_port_element)
{
  wcd_slimbus_direction direction = E_WCD_SLIMBUS_RX;
  uint32 port_number;
  uint32 *used_ports_map = NULL;

  if ( wcd_determine_slimbusport_number(bus_port_element, &port_number, &direction) != E_WCD_SUCCESS)
  {
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (direction == E_WCD_SLIMBUS_RX)
  {
    used_ports_map = &used_rx_bus_ports;
  }
  else
  {
    used_ports_map = &used_tx_bus_ports;
  }

  if (direction == E_WCD_SLIMBUS_RX)
  {
    WCD_DEBUG_INFO_1("=WCD_93XX: <Freeing SBRX %u.>\n", (unsigned int)port_number);
  }
  else if (direction == E_WCD_SLIMBUS_TX)
  {
    WCD_DEBUG_INFO_1("=WCD_93XX: <Freeing SBTX %u.>\n", (unsigned int)port_number);
  }

  //used_bus_ports are (bit0 = port1)
  //check if currently used because we are going to XOR it out
  if (*used_ports_map & (0x1 << (port_number - 1)))
  {
    *used_ports_map = (*used_ports_map) ^ (0x1 << (port_number-1));
  }

  return E_WCD_SUCCESS;

}

/******************************************************************************
 **************                 TX PATH MAPPING                 ***************
 *****************************************************************************/

/**
 * \brief Integer indicating which TX paths have been used.
 */
uint32 used_txpaths = 0;

// Mapping for which ADCs can connect to which decimator
uint32  wcd9306_adc_1_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_adc_2_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_adc_3_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_adc_4_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_adc_5_txpath_map[] = { 3, 4, 0 };

uint32  wcd9302_adc_1_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_adc_2_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_adc_3_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_adc_4_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_adc_5_txpath_map[] = { 0 };

uint32 *adc_1_txpath_map = NULL;
uint32 *adc_2_txpath_map = NULL;
uint32 *adc_3_txpath_map = NULL;
uint32 *adc_4_txpath_map = NULL;
uint32 *adc_5_txpath_map = NULL;

//Mapping for which DMICs can connect to which decimator
uint32  wcd9306_dmic_1_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_dmic_2_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_dmic_3_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_dmic_4_txpath_map[] = { 1, 2, 3, 4, 0 };

uint32  wcd9302_dmic_1_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_dmic_2_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_dmic_3_txpath_map[] = { 0 };
uint32  wcd9302_dmic_4_txpath_map[] = { 0 };

uint32  *dmic_1_txpath_map = NULL;
uint32  *dmic_2_txpath_map = NULL;
uint32  *dmic_3_txpath_map = NULL;
uint32  *dmic_4_txpath_map = NULL;

//Mapping for which decimators can connect to which slimbus
uint32  wcd9306_sbtx_1_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_sbtx_2_txpath_map[] = { 1, 2, 3, 4, 0 };
uint32  wcd9306_sbtx_3_txpath_map[] = { 3, 0 };
uint32  wcd9306_sbtx_4_txpath_map[] = { 4, 0 };
uint32  wcd9306_sbtx_5_txpath_map[] = { 1, 0 };

uint32  wcd9302_sbtx_1_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_sbtx_2_txpath_map[] = { 1, 2, 0 };
uint32  wcd9302_sbtx_3_txpath_map[] = { 0 };
uint32  wcd9302_sbtx_4_txpath_map[] = { 0 };
uint32  wcd9302_sbtx_5_txpath_map[] = { 1, 0 };

uint32  *sbtx_1_txpath_map = NULL;
uint32  *sbtx_2_txpath_map = NULL;
uint32  *sbtx_3_txpath_map = NULL;
uint32  *sbtx_4_txpath_map = NULL;
uint32  *sbtx_5_txpath_map = NULL;

uint32 tx_path_count;
uint32 rx_path_count;
wcd_element** txpath_element_map = NULL;
wcd_element** rxpath_element_map = NULL;

//Mapping for which ANC Channels can connect to which decimator
uint32  anc_channel_1_txpath_map[] = { 3, 0 };
uint32  anc_channel_2_txpath_map[] = { 4, 0 };

#define WCD9306_TX_PATH_COUNT 4

wcd_element* wcd9306_txpath_element_map[] = { &wcd_TX_PATH_1, &wcd_TX_PATH_2, &wcd_TX_PATH_3,
                              &wcd_TX_PATH_4 };

#define WCD9306_RX_PATH_COUNT 4

wcd_element* wcd9306_rxpath_element_map[] = { &wcd_RX_1_PATH, &wcd_RX_2_PATH, &wcd_RX_3_PATH,
                              &wcd_RX_4_PATH };

#define WCD9302_TX_PATH_COUNT 2

wcd_element* wcd9302_txpath_element_map[] = { &wcd_TX_PATH_1, &wcd_TX_PATH_2 };

#define WCD9302_RX_PATH_COUNT 3

wcd_element* wcd9302_rxpath_element_map[] = { &wcd_RX_1_PATH, &wcd_RX_2_PATH, &wcd_RX_3_PATH };


/**
 * \brief Determines the decimator to use based on the source (adc/dmic) and sink (sbtx port)
 */
wcd_result wcd_determine_txpath(wcd_element* source, wcd_element *sink, wcd_element** txpath)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 *txpath_map = NULL;
  uint32 *sbtx_map = NULL;

  uint32 txpath_iterator = 0;
  uint32 sbtx_iterator = 0;

  int32 used_txpath = 0;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Determine TX Path.>\n");

    if (source == NULL || sink == NULL)
    {
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if (source->type == WCD_ADC_TYPE_ID)
    {
      switch(source->id)
      {
        case WCD_ADC_1_ID:
          txpath_map = adc_1_txpath_map;
          break;
        case WCD_ADC_2_ID:
          txpath_map = adc_2_txpath_map;
          break;
        case WCD_ADC_3_ID:
          txpath_map = adc_3_txpath_map;
          break;
        case WCD_ADC_4_ID:
          txpath_map = adc_4_txpath_map;
          break;
        case WCD_ADC_5_ID:
          txpath_map = adc_5_txpath_map;
          break;
      }
    }
    else if (source->type == WCD_DMIC_TYPE_ID)
    {
      switch(source->id)
      {
        case WCD_DMIC_1_ID:
          txpath_map = dmic_1_txpath_map;
          break;
        case WCD_DMIC_2_ID:
          txpath_map = dmic_2_txpath_map;
          break;
        case WCD_DMIC_3_ID:
          txpath_map = dmic_3_txpath_map;
          break;
        case WCD_DMIC_4_ID:
          txpath_map = dmic_4_txpath_map;
          break;
      }
    }
    else if (source->type == WCD_ANC_CHANNEL_TYPE_ID)
    {
      switch(source->id)
      {
        case WCD_ANC_CHANNEL_1_ID:
          txpath_map = anc_channel_1_txpath_map;
          break;
        case WCD_ANC_CHANNEL_2_ID:
          txpath_map = anc_channel_2_txpath_map;
          break;
      }
    }

    if (txpath_map == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unknown source.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    switch(sink->id)
    {
      case WCD_SBTX_1_ID:
        sbtx_map = sbtx_1_txpath_map;
        break;
      case WCD_SBTX_2_ID:
        sbtx_map = sbtx_2_txpath_map;
        break;
      case WCD_SBTX_3_ID:
        sbtx_map = sbtx_3_txpath_map;
        break;
      case WCD_SBTX_4_ID:
        sbtx_map = sbtx_4_txpath_map;
        break;
      case WCD_SBTX_5_ID:
        sbtx_map = sbtx_5_txpath_map;
        break;
    }

    if (sbtx_map == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unknown sink.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    //loop through decimator map and see if its in the sbtx map
    while(txpath_map[txpath_iterator] != 0 && used_txpath == 0)
    {
      sbtx_iterator = 0;
      while (sbtx_map[sbtx_iterator] != 0)
      {
        if (txpath_map[txpath_iterator] != sbtx_map[sbtx_iterator])
        {
          sbtx_iterator++;
          continue;
        }

        //check if the TX Path is used
        if (used_txpaths & (0x1 << (txpath_map[txpath_iterator] - 1)))
        {
          sbtx_iterator++;
          continue;
        }

        //its not used, save it
        used_txpath = txpath_map[txpath_iterator];
        break;
      }

      txpath_iterator++;
    }

    if (used_txpath == 0)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <No TX Paths available for the SBTX port and ADC/DMIC/ANC Channel.>\n");
      return_code = E_WCD_UNABLE_TO_ROUTE;
      break;
    }

    WCD_DEBUG_INFO_1("=WCD_93XX: <Determined TX Path: %u.>\n", (unsigned int)used_txpath);
    used_txpaths = used_txpaths | ( 0x1 << (used_txpath -1) );
    *txpath = txpath_element_map[used_txpath - 1];

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Determine TX Path. RC:%d.>\n", return_code);
  return return_code;
}

/**
 * \brief Marks a decimator as freed and allows other connections
 */
wcd_result wcd_free_txpath(wcd_element* tx_path)
{
  uint32 tx_path_number = 0;
  uint32 iterator = 0;

  while(iterator < tx_path_count)
  {
    if (txpath_element_map[iterator] == tx_path)
    {
      tx_path_number = iterator + 1;
      break;
    }

    iterator++;
  }

  if (tx_path_number == 0)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Attempted to free TX Path. Got unknown element.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  WCD_DEBUG_INFO_1("=WCD_93XX: <Freeing TX Path %u.>\n", (unsigned int)tx_path_number);

  //check if currently used because we are going to XOR it out
  if (used_txpaths & (0x1 << (tx_path_number - 1)))
  {
    used_txpaths = used_txpaths ^ (0x1 << (tx_path_number - 1));
  }

  return E_WCD_SUCCESS;
}

/**
 * \brief function required to identify ADC for Helicon
 */
wcd_result wcd_find_ADC_Path (wcd_element** pCurrElement, wcd_element* pSrcElement, uint32 elem_type)
{
  HANDLE_UNUSED_PARAMETER(pCurrElement);
  HANDLE_UNUSED_PARAMETER(pSrcElement);
  HANDLE_UNUSED_PARAMETER(elem_type);

  return E_WCD_SUCCESS;
}

/******************************************************************************
 **************                   ROUTE STACKS                  ***************
 *****************************************************************************/

uint32 rs_dmic_sbtx[] = { WCD_BUS_PORT_TYPE_ID, WCD_MUX_TYPE_ID, WCD_TX_PATH_TYPE_ID,
                          WCD_HPF_TYPE_ID, WCD_MUX_TYPE_ID,
                          WCD_ROUTE_STACK_END };


uint32 rs_adc_sbtx[] =  { WCD_BUS_PORT_TYPE_ID, WCD_MUX_TYPE_ID, WCD_TX_PATH_TYPE_ID,
                          WCD_HPF_TYPE_ID, WCD_MUX_TYPE_ID,
                          WCD_ROUTE_STACK_END };

uint32 rs_anc_channel_sbtx[] = { WCD_BUS_PORT_TYPE_ID, WCD_MUX_TYPE_ID, WCD_TX_PATH_TYPE_ID,
                          WCD_HPF_TYPE_ID, WCD_MUX_TYPE_ID,
                          WCD_ROUTE_STACK_END };

uint32 *rs_sbrx_earout = NULL;

uint32 wcd9306_rs_sbrx_earout_[] = { WCD_EAR_PA_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_COMPANDER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_earout[] = { WCD_EAR_PA_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_RX_PATH_TYPE_ID,
                            WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 *rs_sbrx_earout_ultrasound = NULL;

uint32 wcd9306_rs_sbrx_earout_ultrasound[] = { WCD_EAR_PA_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_SPECIAL_TYPE | WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_COMPANDER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_earout_ultrasound[] = { WCD_EAR_PA_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_SPECIAL_TYPE | WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID,  WCD_RX_PATH_TYPE_ID,
                            WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 *rs_sbrx_hph_l = NULL;

uint32 wcd9306_rs_sbrx_hph_l[] =    { WCD_DAC_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_COMPANDER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_hph_l[] =    { WCD_DAC_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_RX_PATH_TYPE_ID,
                            WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 *rs_sbrx_hph_r = NULL;

uint32 wcd9306_rs_sbrx_hph_r[] =    { WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_COMPANDER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_hph_r[] =    { WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_RX_PATH_TYPE_ID,
                            WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 *rs_sbrx_lo1 = NULL;
uint32 *rs_sbrx_lo2 = NULL;

uint32 wcd9306_rs_sbrx_lo1[] =    { WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_COMPANDER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9306_rs_sbrx_lo2[] =    { WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_COMPANDER_TYPE_ID,
                            WCD_MIXER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_lo1[] =    { WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_lo2[] =    { WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 *rs_sbrx_spkr = NULL;

uint32 wcd9306_rs_sbrx_spkr[] =   { WCD_DAC_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_CLASS_CONTROLLER_TYPE_ID, WCD_COMPANDER_TYPE_ID,
                            WCD_MIXER_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID, WCD_MIXER_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 wcd9302_rs_sbrx_spkr[] =   { WCD_DAC_TYPE_ID, WCD_RX_BIAS_TYPE_ID,
                            WCD_RX_PATH_TYPE_ID, WCD_HPF_TYPE_ID,
                            WCD_MIXER_TYPE_ID, WCD_MUX_TYPE_ID,
                            WCD_ROUTE_STACK_END };

uint32 rs_lo2_differential[] = {  WCD_DAC_TYPE_ID, WCD_MUX_TYPE_ID,
                                  WCD_MUX_TYPE_ID,
                                  WCD_ROUTE_STACK_END };



/**
 * \brief Determines and populates the route stack from the source to the sink
 */
wcd_result wcd_determine_route_stack(wcd_source source, wcd_sink sink, uint32** p_route_stack)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32  source_category = source & CATEGORY_MASK;

  uint32  sink_category   = sink & CATEGORY_MASK;
  uint32  sink_id         = sink & (~CATEGORY_MASK);

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Determining route stack.>\n");

    if(p_route_stack == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Route stack handle point is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    *p_route_stack = NULL;

    if ( (source_category == SOURCE_AUDIO_INPUT_MASK) &&
         (sink_category == SINK_SLIMBUS_TX_MASK ||sink_category == SINK_I2S_TX_INPUT_MASK) )
    {
      *p_route_stack = rs_adc_sbtx;
    }
    else if ( (source_category == SOURCE_DMIC_INPUT_MASK) &&
         (sink_category == SINK_SLIMBUS_TX_MASK ||sink_category == SINK_I2S_TX_INPUT_MASK) )
    {
      *p_route_stack = rs_dmic_sbtx;
    }
    else if ( (source_category == SOURCE_ANC_CHANNEL_MASK) &&
        (sink_category == SINK_SLIMBUS_TX_MASK ||sink_category == SINK_I2S_TX_INPUT_MASK) )
    {
      *p_route_stack = rs_anc_channel_sbtx;
    }
    else if ( source_category == SOURCE_SLIMBUS_RX_INPUT_MASK ||
              source_category == SOURCE_I2S_RX_INPUT_MASK ||
              source_category == SOURCE_REUSE_BUS_PORT_MASK )
    {
      if ( sink_category == SINK_SPECIAL_SINK_MASK )
      {
        switch(sink_id)
        {
          case SINK_EAROUT_ID:
            *p_route_stack = rs_sbrx_earout;
            break;
          case SINK_ULTRASOUND_EAROUT_ID:
            *p_route_stack = rs_sbrx_earout_ultrasound;
            break;
          case SINK_SPEAKER_ID:
            *p_route_stack = rs_sbrx_spkr;
            break;
          case SINK_HPHL_ID:
            *p_route_stack = rs_sbrx_hph_l;
            break;
          case SINK_HPHR_ID:
            *p_route_stack = rs_sbrx_hph_r;
            break;
        }
      }
      else if ( sink_category == SINK_LINE_MASK)
      {
        switch(sink_id)
        {
          case 1:
            *p_route_stack = rs_sbrx_lo1;
            break;
          case 2:
            *p_route_stack = rs_sbrx_lo2;
            break;
        }
      }
    }

    if(*p_route_stack == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unknown source and sink match. Could not find a route stack.>\n");
      return_code = E_WCD_UNABLE_TO_ROUTE;
      break;
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Determining route stack. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Determines and populates the route stack from the source to the sink for the differential connection
 */
wcd_result wcd_determine_differential_route_stack(wcd_sink sink, uint32** p_route_stack)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32  sink_category   = sink & CATEGORY_MASK;
  uint32  sink_id         = sink & (~CATEGORY_MASK);

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Determining differential route stack.>\n");

    if(p_route_stack == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Route stack handle point is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    *p_route_stack = NULL;

    if ( sink_category == SINK_LINE_MASK)
    {
      if (sink_id == 1)
      {
        *p_route_stack = rs_lo2_differential;
      }
    }


    if(*p_route_stack == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unknown source and sink match for diferential. Could not find a route stack.>\n");
      return_code = E_WCD_UNABLE_TO_ROUTE;
      break;
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Determining differential route stack. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Handles any special routing not posible by the common code. Mainly for
 * ultrasound earout routing.
 */
wcd_result wcd_handle_special_routing(wcd_route* route, uint32 special_route_stack_type)
{
  if (special_route_stack_type != (WCD_ROUTE_STACK_SPECIAL_TYPE | WCD_RX_BIAS_TYPE_ID))
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Got a special routing type. Not expected earout ultrasound/RX Bias.>\n");
    return E_WCD_UNABLE_TO_ROUTE;
  }

  //we need to route to the RX Path 7 RX Bias for earout
  if (route == NULL || route->elements == NULL || route->elements->head == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Special routing. A required parameter is NULL.>\n");
    return E_WCD_UNABLE_TO_ROUTE;
  }

  //verify top of route stack is wcd_RDAC_1_MUX
  if ( (wcd_element*)route->elements->head->data != &wcd_RDAC_1_MUX)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Special routing. Top of route stack is not wcd_RDAC_1_MUX.>\n");
    return E_WCD_UNABLE_TO_ROUTE;
  }

  return wcd_push_to_list(route->elements, &wcd_RX_BIAS_4); /* KTODO  wcd_RX_BIAS_7 */
}



/******************************************************************************
 **************          INITIALIZATION/DEINITIALIZATION        ***************
 *****************************************************************************/

uint32    usecase_enabled_count = 0;
wcd_bool  reset_required = WCD_TRUE;

//#ifdef WCD_MBHC_ENABLE

wcd_result wcd_initialize_common(void)
{
    wcd_result return_code = E_WCD_SUCCESS;
    rm_params rm_params_send;

    return_code = wcd_activate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("-WCD_93XX: <Could not activate bus before Resource Manager clock bring-up sequence> \n");
      return return_code;
    }

    WCD_DEBUG_INFO_0("=WCD_93XX: <Calling Audio_Resource_Manager to bring-up Ext Mclk> \n");
    rm_params_send.resource_id = SHARED_RESOURCE_CLOCK_BLOCK;
    rm_params_send.action = RM_ENABLE;
    return_code = rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_MANAGE_RESOURCE, &rm_params_send);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error in Resource Manager clock bring-up> \n");
    }

    return_code = wcd_deactivate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not deactivate bus after Resource Manager clock bring-up sequence> \n");
    }

    return return_code;
}

wcd_result wcd_deinitialize_common(void)
{
    wcd_result return_code = E_WCD_SUCCESS;
    rm_params rm_params_send;

    return_code = wcd_activate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("-WCD_93XX: <Could not activate bus before Resource Manager clock tear-down sequence> \n");
      return return_code;
    }

    WCD_DEBUG_INFO_0("=WCD_93XX: <Calling Audio_Resource_Manager to tear-down Ext Mclk> \n");
    rm_params_send.resource_id = SHARED_RESOURCE_CLOCK_BLOCK;
    rm_params_send.action = RM_DISABLE;
    return_code = rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_MANAGE_RESOURCE, &rm_params_send);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error in Resource Manager clock tear-down> \n");
    }

    return_code = wcd_deactivate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not deactivate bus after Resource Manager clock tear-down sequence> \n");
    }

    return return_code;
}

//#else
#if 0
wcd_result wcd_initialize_common(void)
{
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_activate_bus();

  /** Central Bandgap **/

  // #BIAS_CURR_CTL_2__PROGRAM_THE_0P85V_VBG_REFERENCE = V_0P868V
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x104, 0x0C, 0, 0x04);
  // #BIAS_CENTRAL_BG_CTL__REGISTER_PRECHARGE = ON
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x80, 0, 0x80);
  // #BIAS_CENTRAL_BG_CTL__DC_START_UP_EN = ENABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x04, 0, 0x04);
  // #BIAS_CENTRAL_BG_CTL__BG_EN = ENABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x01, 0, 0x01);
  // #Delay_mS 1
  WCD_SLEEP(1);
  // #BIAS_CENTRAL_BG_CTL__REGISTER_PRECHARGE = OFF
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x80, 0, 0x00);


  /** Clock block **/

  // #CLK_BUFF_EN1__CLK_BUFF_EN = ENABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x108, 0x01, 0, 0x01);
  // #Delay_mS 1
  WCD_SLEEP(1);
  // #CLK_BUFF_EN2__ASYNC_RESET_BLANK_CKT = NORMAL_OP
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x02, 0, 0x00);
  // #CLK_BUFF_EN2__MCLK_EN = ENABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x04, 0, 0x04);
  // #Delay_mS 1
  WCD_SLEEP(1);

  /** Connection Clock **/

  // #CDC_CLK_OTHR_CTL__CONN_CLK_EN = ENABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x30C, 0x04, 0, 0x04);

  /** I2C Clocks - only for I2C bus mode **/
  if (wcd_init_options.bustype == E_WCD_I2C)
  {
    // #CDC_CLK_RX_I2S_CTL__RX_I2S_CLK_EN = ENABLE
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x306, 0x10, 0, 0x10);
    // #CDC_CLK_TX_I2S_CTL__TX_I2S_CLK_EN = TX_I2S_CLK_EN
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x307, 0x10, 0, 0x10);
  }
  wcd_bus_flush(E_WCD_CODEC_MAIN);

  return_code = wcd_deactivate_bus();

  return return_code;
}

wcd_result wcd_deinitialize_common(void)
{
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_activate_bus();

  /** Connection Clock **/

  // #CDC_CLK_OTHR_CTL__CONN_CLK_EN = DISABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x30C, 0x04, 0, 0x00);

  /** I2C Clocks -- only for I2C bus mode **/
  if (wcd_init_options.bustype == E_WCD_I2C)
  {
    // #CDC_CLK_RX_I2S_CTL__RX_I2S_CLK_EN = DISABLE
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x306, 0x10, 0, 0x00);
    // #CDC_CLK_TX_I2S_CTL__TX_I2S_CLK_EN = TX_I2S_CLK_DIS
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x307, 0x10, 0, 0x00);
  }

  /** Clock block **/

  // #CLK_BUFF_EN2__MCLK_EN = DISABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x04, 0, 0x00);
  // #Delay_mS 1
  WCD_SLEEP(1);
  // #CLK_BUFF_EN2__ASYNC_RESET_BLANK_CKT = ASYNC_RESET
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x02, 0, 0x02);
  // #CLK_BUFF_EN1__CLK_BUFF_EN = DISABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x108, 0x01, 0, 0x00);

  /** Central Bandgap **/

  // #BIAS_CENTRAL_BG_CTL__BG_EN = DISABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x01, 0, 0x00);
  // #BIAS_CENTRAL_BG_CTL__DC_START_UP_EN = DISABLE
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x04, 0, 0x00);

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  return_code = wcd_deactivate_bus();

  return return_code;
}

#endif



/**
 * \brief Initializes the codec before enabling any usecases.
 */
wcd_result wcd_initialize_codec(void)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_vdd_buck_voltage vdd_buck;
  wcd_reset_codec_payload_struct reset_codec_payload;

  if (reset_required == WCD_TRUE)
  {
    return_code = wcd_get_vdd_buck_voltage(&vdd_buck);
    reset_codec_payload.buck_voltage = vdd_buck;
    return_code = wcd_reset_codec(&reset_codec_payload);
  }

  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  if (usecase_enabled_count == 0)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <Begin codec initialization.>\n");

    // AUDIO_RESOURCE_MANAGER
    wcd_initialize_common();

    WCD_DEBUG_INFO_0("=WCD_93XX: <End codec initialization.>\n");

  }

  usecase_enabled_count++;

  return return_code;
}

/**
 * \brief Deinitializes the codec once all the usecases have been disabled
 */
wcd_result wcd_deinitialize_codec(void)
{
  wcd_result return_code = E_WCD_SUCCESS;

  if( usecase_enabled_count > 0 )
  {
    usecase_enabled_count--;
  }

  if (usecase_enabled_count == 0)
  {
    // AUDIO_RESOURCE_MANAGER
    wcd_deinitialize_common();
  }

  return return_code;
}

/**
 * \brief Any target specific register writes that need to be done before enabling a particular (or any) usecase
 */
wcd_result wcd_pre_enable(wcd_usecase *p_usecase)
{
  //wcd_connection_info* connection = NULL;
  //wcd_list_element* iterator = NULL;
  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("+WCD_93XX: <WCD pre-enable processing.>\n");

  HANDLE_UNUSED_PARAMETER(p_usecase);
  HANDLE_UNUSED_PARAMETER(connection);
  HANDLE_UNUSED_PARAMETER(iterator);

  do
  {

  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD pre-enable processing. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Any target specific register writes that need to be done after disabling a particular (or any) usecase
 */
wcd_result wcd_post_disable(wcd_usecase *p_usecase)
{
  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("+WCD_93XX: <WCD post-disable processing.>\n");

  HANDLE_UNUSED_PARAMETER(p_usecase);

  do
  {
  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD post-disable processing. RC:%d.>\n", return_code);

  return return_code;
}


/******************************************************************************
 **************                    RESET CODEC                  ***************
 *****************************************************************************/

/**
 * \brief Marks that a reset will be required before the codec is initialized.
 */
wcd_result wcd_reset_required(void)
{
  reset_required = WCD_TRUE;

  return E_WCD_SUCCESS;
}

/**
 * \brief Reset all counters in the driver.
 */
wcd_result reset_counters(void)
{
  WCDH_RESET();

  usecase_enabled_count = 0;

  return E_WCD_SUCCESS;
}

#define WCD_RESET_TIMEOUT_MS    100
#define WCD_RESET_MAX_ATTEMPT   5
#define WCD_QFUSE_STATUS_QFUSE_SENSE_DONE 0x3

/**
 * \brief Soft resets the codec.
 */
wcd_result wcd_reset_codec(void* reset_payload)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 reset_attempt_counter = 0;
  wcd_reset_codec_payload_struct reset_codec_payload;
  uint8 qfuse_status = 0;
  wcd_codec_info codec_info;

  if (reset_payload != NULL)
  {
    reset_codec_payload.buck_voltage = ((wcd_reset_codec_payload_struct*)reset_payload)->buck_voltage;
    wcd_store_codec_buck_voltage(reset_codec_payload.buck_voltage);
  }
  else
  {
    reset_codec_payload.buck_voltage = E_WCD_VDD_BUCK_2P15V;
    wcd_store_codec_buck_voltage(E_WCD_VDD_BUCK_2P15V);
  }

  for(reset_attempt_counter = 0; reset_attempt_counter < WCD_RESET_MAX_ATTEMPT; reset_attempt_counter++)
  {
    do
    {
      WCD_DEBUG_INFO_1("=WCD_93XX: <Reset codec. Attempt %u.>\n", (unsigned int)reset_attempt_counter);

      return_code = wcd_get_codec_info(&codec_info);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error while obtaining codec information.>\n");
        break;
      }

      //reset the shadow register here
      WCD_DEBUG_INFO_0("=WCD_93XX: <Resetting register memory shadow copy.>\n");
      return_code = wcd_bus_init_shadow_register(codec_info);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing the register memory shadow copy.>\n");
        return return_code;
      }

      //reset all the counters
      return_code = reset_counters();
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error resetting counters.>\n");
        break; //while loop
      }

      return_code = wcd_activate_bus();
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error activating bus before resetting.>\n");
        break; //while loop
      }

      /**************************************************************
      *       CODEC RESET SEQUENCE FROM HW/SW DOCUMENT              *
      *************************************************************/


      WCD_DEBUG_INFO_0("=WCD_93XX: <Begin reset codec sequence.>\n");

      //Reset sequence: Figure 3-4 HW/SW Doc

      //CDC_CTL CDC_DIG_RST_N = RESET_THE_DIGITAL_CODEC
      //CDC_CTL CDC_ANA_RST_N = RESET_THE_ANALOG_CODEC
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x80, 0x01, 0, 0x0);  
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //LEAKAGE_CTL GDFS_CLAMP_EN = TRUE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x88, 0x04, 0x0, 0x04);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //LEAKAGE_CTL GDFS_EN_REST = FALSE
      //LEAKAGE_CTL GDFS_EN_FEW = FALSE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x88, 0x03, 0x0, 0x0);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      wcd_bus_flush(E_WCD_CODEC_MAIN);
      //DELAY 5MS
      //LOW LEAKAGE STATE
      WCD_SLEEP(5);

      //LEAKAGE_CTL GDFS_EN_FEW = TRUE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x88, 0x01, 0x0, 0x1);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      wcd_bus_flush(E_WCD_CODEC_MAIN);
      //DELAY 1MS
      WCD_SLEEP(1);

      //LEAKAGE_CTL GDFS_EN_REST = TRUE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x88, 0x02, 0x0, 0x2);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      wcd_bus_flush(E_WCD_CODEC_MAIN);
      //DELAY 1MS
      WCD_SLEEP(1);

      //LEAKAGE_CTL GDFS_CLAMP_EN = FALSE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x88, 0x04, 0x0, 0x0);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      wcd_bus_flush(E_WCD_CODEC_MAIN);
      //DELAY 1MS
      WCD_SLEEP(1);

      //CDC_CTL CDC_DIG_RST_N = REMOVES_THE_DIGITAL_CODEC_FROM_RESET
      //CDC_CTL CDC_ANA_RST_N = REMOVES_THE_ANALOG_CODEC_FROM_RESET
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x80, 0x03, 0x0, 0x3);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      wcd_bus_flush(E_WCD_CODEC_MAIN);

      //DELAY 1MS
      WCD_SLEEP(1);

      /*#BIAS_CENTRAL_BG_CTL__REGISTER_PRECHARGE = ON
      0x101; 0x80; 0x80
      #BIAS_CENTRAL_BG_CTL__DC_START_UP_EN = ENABLE
      0x101; 0x04; 0x04
      #BIAS_CENTRAL_BG_CTL__BG_EN = ENABLE
      0x101; 0x01; 0x01
      #Delay_mS 1
      #BIAS_CENTRAL_BG_CTL__REGISTER_PRECHARGE = OFF
      0x101; 0x80; 0x00
      */
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x80, 0, 0x80);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x04, 0, 0x04);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x01, 0, 0x01);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x80, 0, 0x00);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      // #CHIP_CTL__LP_MODE = DISABLED
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x00, 0x08, 3, 0x0);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      switch( wcd_init_options.mclk_speed )
      {
        case E_WCD_9_6_MHZ:
          {
            // #CHIP_CTL__DIG_MCLK_FREQ = F_9P6MHZ
            return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x00, 0x06, 1, 0x1);
            WCD_DEBUG_ERROR_0("=WCD_93XX: <mclk.speed=9.6mhz.>\n");
          }
          break;
        case E_WCD_12_288_MHZ:
          {
            // #CHIP_CTL__DIG_MCLK_FREQ = F_12P288MHZ
            return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x00, 0x06, 1, 0x0);
            WCD_DEBUG_ERROR_0("=WCD_93XX: <mclk.speed=12.288mhz.>\n");
          }
          break;
        default:
          return_code = E_WCD_INITIALIZATION_ERROR;
      }

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      /*#CLK_BUFF_EN1__CLK_BUFF_EN = ENABLE
        0x108; 0x01; 0x01
        #Delay_mS 1
        #CLK_BUFF_EN2__ASYNC_RESET_BLANK_CKT = NORMAL_OP
        0x109; 0x02; 0x00
        #CLK_BUFF_EN2__MCLK_EN = ENABLE
        0x109; 0x04; 0x04
      */
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x108, 0x1, 0, 0x01);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      WCD_SLEEP(1);
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x02, 0, 0x00);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x04, 0, 0x04);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //#QFUSE_CTL__SENSE_EN_0 = 1
      //0x048; 0x01; 0x01
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x048, 0x1, 0, 0x1);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //#QFUSE_CTL__SENSE_EN_1 = 1
      //0x048; 0x02; 0x02
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x048, 0x02, 0, 0x2);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //DELAY 1MS
      WCD_SLEEP(1);

      // read qfuse data registers to determine codec tier
      return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)0x49, 0x3, 0, (uint8*)&qfuse_status);
     /* if(qfuse_status != WCD_QFUSE_STATUS_QFUSE_SENSE_DONE)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error in initialization sequence as QFUSE DONE bits are not set.>\n");
        return_code = E_WCD_QFUSE_NOT_DONE_FAILURE;
        break;
      } */  //commenting out....

      /*#CLK_BUFF_EN2__MCLK_EN = DISABLE
        0x109; 0x04; 0x00
        #Delay_mS 1
        #CLK_BUFF_EN2__ASYNC_RESET_BLANK_CKT = ASYNC_RESET
        0x109; 0x02; 0x02
        #CLK_BUFF_EN1__CLK_BUFF_EN = DISABLE
        0x108; 0x01; 0x00
        #BIAS_CENTRAL_BG_CTL__BG_EN = DISABLE
        0x101; 0x01; 0x00
        #BIAS_CENTRAL_BG_CTL__DC_START_UP_EN = DISABLE
        0x101; 0x04; 0x00
      */
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x04, 0, 0x00);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      WCD_SLEEP(1);
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x109, 0x02, 0, 0x02);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x108, 0x01, 0, 0x00);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x01, 0, 0x00);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x101, 0x04, 0, 0x00);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      // #CDC_CLK_POWER_CTL__GRP_A = ENABLE
      // #CDC_CLK_POWER_CTL__GRP_B = ENABLE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x314, 0x03, 0, 0x03);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      // #CDC_CLK_MCLK_CTL__MCLK_EN = ENABLE
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x311, 0x01, 0, 0x01);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register 0x311.>\n");
        break; //while loop
      }

      WCD_DEBUG_ERROR_0("=WCD_93XX: wcd_mclk_value 0x311\n");

      wcd_bus_flush(E_WCD_CODEC_MAIN);

      WCD_DEBUG_INFO_0("=WCD_93XX: <End reset codec sequence.>\n");

      /**************************************************************
      * INSERT ANY GLOBAL SETTINGS HERE TO AVOID CHANGING PROFILES *
      *************************************************************/

      WCD_DEBUG_INFO_0("=WCD_93XX: <Begin global settings.>\n");


      //5. Ensure the DEM#1is using the ANC delay buffer
      //a. Set bit 5 (DEM_INP_SEL2) of reg 0x2B5 (CDC_RX1_B6_CTL) to 1 (CLASSH_H_BUFFER)
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x2B5, 0x20, 5, 0x1);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //5. Ensure the DEM#12s using the ANC delay buffer
      //a. Set bit 5 (DEM_INP_SEL2) of reg 0x2BD (CDC_RX2_B6_CTL) to 1 (CLASSH_H_BUFFER)
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x2BD, 0x20, 5, 0x1);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }


//#ifdef WCD_MBHC_ENABLE
      //0x110; 0x0C; 0x0C
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x110, 0x0C, 0, 0x0C);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //0x12F; 0xFC; 0x60
      return_code |= wcd_register_write(E_WCD_CODEC_MAIN, 0x12F, 0xFC, 0, 0x60);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      wcd_bus_flush(E_WCD_CODEC_MAIN);
//#endif // WCD_MBHC_ENABLE

      if(codec_info.version == E_WCD_VER_1P0)
      {
        //#RX_HPH_CHOP_CTL_CLK_INV = 1
        //0x1A5; 0x40; 0x40
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1A5, 0x40, 6, 0x1);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }

        //#BUCK_CTRL_CCL_4__IPK_FRC_RST = C_150FF
        //0x18C; 0x0C; 0x04
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x18C, 0x0C, 0x2, 0x1);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }

        //#RX_BUCK_BIAS1__INZ_IPK_BIAS_CURR = I_4P0_UA
        //0x1C7; 0xF0; 0x80
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1C7, 0xF0, 4, 0x8);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      if(codec_info.version == E_WCD_VER_2P0)
      {
        //#RX_BUCK_BIAS1__INZ_IPK_BIAS_CURR = I_3P0_UA
        //0x1C7; 0xF0; 0x60
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1C7, 0xF0, 4, 0x6);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      if(reset_codec_payload.buck_voltage == E_WCD_VDD_BUCK_1P8V ||
         reset_codec_payload.buck_voltage == E_WCD_VDD_BUCK_1P8V_2P15V )
      {
        /*
        #BUCK_MODE_2__VTH_BYP_PFM = 10
        0x182; 0xF0; 0xA0
        #BUCK_MODE_2__VTH_BYP_PWM = 10
        0x182; 0x0F; 0x0A
        */
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x182, 0xF0, 0x4, 0xA);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x182, 0x0F, 0x0, 0xA);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      //#NCP_CLK__IN_CLK_NOV_TIME = T_45_NS
      //0x193; 0x1C; 0x1C
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x193, 0x1C, 0x0, 0x1C);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }
      //#NCP_CLK__OUT_CLK_NOV_TIME = T_45_NS
      //0x193; 0xE0; 0xE0
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x193, 0xE0, 0x0, 0xE0);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      if(codec_info.version == E_WCD_VER_2P0)
      {
        //#BUCK_CTRL_CCL_3__MIN_NON = C_104FF
        //0x18B; 0x03; 0x03
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x18B, 0x03, 0x0, 0x03);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }

        //#BUCK_CTRL_CCL_3__MIN_PON = C_104FF
        //0x18B; 0x0C; 0x0C
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x18B, 0x0C, 0x0, 0x0C);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      //#BUCK_MODE_3__DISABLE_MODE_SEL = DISABLE_PWM_MODE
      //0x183; 0x03; 0x02
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x183, 0x03, 0x0, 0x02);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //#RX_EAR_BIAS_PA__DAC_BIAS_CURR = I_3P5_UA
      //0x1BA; 0xF0; 0x70
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1BA, 0xF0, 0x0, 0x70);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //#RX_HPH_BIAS_PA__DAC_BIAS_CURR = I_3P5_UA
      //0x1A6; 0xF0; 0x70
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1A6, 0xF0, 0x0, 0x70);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      //#RX_LINE_BIAS_PA__DAC_BIAS_CURR = I_3P5_UA
      //0x1C6; 0xF0; 0x70
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1C6, 0xF0, 0x0, 0x70);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
        break; //while loop
      }

      /* If vdd buck voltage is 1.8V then static gain offset should be set to -3dB */
      if(reset_codec_payload.buck_voltage == E_WCD_VDD_BUCK_1P8V ||
         reset_codec_payload.buck_voltage == E_WCD_VDD_BUCK_1P8V_2P15V )
      {
        //#CDC_COMP1_B4_CTL__STATIC_GAIN_OFFSET = 1
        //0x373; 0x80; 0x80
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x373, 0x80, 0, 0x80);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      if(codec_info.version == E_WCD_VER_1P0 &&
        (reset_codec_payload.buck_voltage == E_WCD_VDD_BUCK_2P15V || reset_codec_payload.buck_voltage == E_WCD_VDD_BUCK_1P8V_2P15V))
      {
        //#RX_EAR_BIAS_PA__PA_BIAS_CURR = I_3_UA
        //0x1BA; 0x0F; 0x03
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x1BA, 0x0F, 0, 0x03);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      if(codec_info.version == E_WCD_VER_2P0)
      {
        //#NCP_DTEST__CKOUT_PHASE_SEL = DELAY_TWO_CKI_CYCLE
        // 0x198; 0x30; 0x20
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x198, 0x30, 0, 0x20);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }

        //#TX_COM_BIAS__AAF_FILT_MODE = CLK_IS_6P144_MHZ
        //0x14C; 0x10; 0x00
        return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x14C, 0x10, 0, 0x0);
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error writing register.>\n");
          break; //while loop
        }
      }

      // Enable MicBias Pulldown
      return_code  = wcd_register_write(E_WCD_CODEC_MAIN, 0x12B, 0x1, 0, 0x1);
#ifndef WCD_MBHC_ENABLE
//      return_code |= wcd_register_write(E_WCD_CODEC_MAIN, 0x131, 0x1, 0, 0x1);//disable mic bias pull down
#endif
      return_code |= wcd_register_write(E_WCD_CODEC_MAIN, 0x137, 0x1, 0, 0x1);

      //#RX_HPH_L_GAIN__GAIN_SOURCE_SEL = REGISTER
      //0x1AE; 0x20; 0x20
      //#RX_HPH_R_GAIN__GAIN_SOURCE_SEL = REGISTER
      //0x1B4; 0x20; 0x20
      //#RX_LINE_1_GAIN__GAIN_SOURCE_SEL = REGISTER
      //0x1CD; 0x20; 0x20
      //#RX_LINE_2_GAIN__GAIN_SOURCE_SEL = REGISTER
      //0x1D1; 0x20; 0x20
      //#SPKR_DRV_GAIN__PA_GAIN_SEL = REGISTER
      //0x1E0; 0x04; 0x04
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1AE, 0x20, 5, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1B4, 0x20, 5, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1CD, 0x20, 5, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1D1, 0x20, 5, 0x01);
      wcd_register_write(E_WCD_CODEC_MAIN, 0x1E0, 0x04, 2, 0x01);

      wcd_bus_flush(E_WCD_CODEC_MAIN);

      WCD_DEBUG_INFO_0("=WCD_93XX: <End global settings.>\n");

    } while(WCD_G_FALSE_BOOLEAN); //while loop

    wcd_deactivate_bus();

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error during reset attempt. Sleeping.>\n");
      WCD_SLEEP(WCD_RESET_TIMEOUT_MS);
    }
    else
    {
      reset_required = WCD_FALSE;
      break; //for loop
    }
  } //for loop

  return return_code;
}

/******************************************************************************
 **************                     CODEC INFO                  ***************
 *****************************************************************************/

#define WCD_CODEC_ID_BYTE0_REG  0x04
#define WCD_CODEC_ID_BYTE1_REG  0x05
#define WCD_CODEC_ID_BYTE2_REG  0x06
#define WCD_CODEC_ID_BYTE3_REG  0x07

/*  CHIP_ID = CHIP_ID_BYTE3 | CHIP_ID_BYTE2 | CHIP_ID_BYTE1 | CHIP_ID_BYTE0 */
#define WCD_WCD9306_1P0_CHIP_ID 0x01030000
#define WCD_WCD9306_2P0_CHIP_ID 0x01030001
#define WCD_WCD9302_1P0_CHIP_ID WCD_WCD9306_1P0_CHIP_ID
#define WCD_WCD9302_2P0_CHIP_ID WCD_WCD9306_2P0_CHIP_ID

//Note bits:
//[6:5] lsb of I2C slave ID, 1 = address 0x0D
//[4:0] wcd9306/02 Version, 0 = wcd9306/02 v1.0; 1 = wcd9306/02 v2.0
#define TAPAN_1P0_VERSION 0x00
#define TAPAN_2P0_VERSION 0x01

#define WCD_CODEC_VER_REG   0x08

#define WCD_CODEC_QFUSE_DATA_OUT1_REG   0x4B
#define WCD_CODEC_QFUSE_DATA_OUT2_REG   0x4C

/**
 * \brief Returns the codec info.
 */
wcd_result wcd_get_codec_info(wcd_codec_info* p_codec_info)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 read_byte0 = 0, read_byte1 = 0;
  uint8 read_byte2 = 0, read_byte3 = 0;
  uint8 chip_version;
  uint32 chip_id;

  do
  {
    if (p_codec_info == NULL)
    {
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    //check if codec info was initialized before
    if (g_wcd_codec_info.codec == E_WCD_CODEC_32B_ANCHOR)
    {
      return_code = wcd_activate_bus();
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error activating bus before resetting.>\n");
        break; //while loop
      }

      // read the codec chip id
      return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_ID_BYTE0_REG, 0xFF, 0, (uint8*)&read_byte0);
      return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_ID_BYTE1_REG, 0xFF, 0, (uint8*)&read_byte1);
      return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_ID_BYTE2_REG, 0xFF, 0, (uint8*)&read_byte2);
      return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_ID_BYTE3_REG, 0xFF, 0, (uint8*)&read_byte3);

      chip_id = ((read_byte3 << 24) | (read_byte2 << 16) | (read_byte1 << 8) | read_byte0);

      // read chip version. Currently chip version is unused as chip id is determine chip version as well.
      return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_VER_REG, 0x1F, 0, (uint8*)&chip_version);

      wcd_deactivate_bus();
      switch(chip_id)
      {
        // Note: New chip version need to be added as and when new codec version will tapeout
        case WCD_WCD9306_1P0_CHIP_ID:
          g_wcd_codec_info.codec = E_WCD_WCD9306;
          g_wcd_codec_info.version = E_WCD_VER_1P0;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Tapan Codec version 1.0. Detected>\n");
          break;
        case WCD_WCD9306_2P0_CHIP_ID:
          g_wcd_codec_info.codec = E_WCD_WCD9306;
          g_wcd_codec_info.version = E_WCD_VER_2P0;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Tapan Codec version 2.0. Detected>\n");
          break;
        default:
          return_code = E_WCD_UNABLE_TO_READ_CODEC_INFO;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Not supported codec detected.>\n");
      }
    }

    p_codec_info->codec = g_wcd_codec_info.codec;
    p_codec_info->version = g_wcd_codec_info.version;

  } while(WCD_G_FALSE_BOOLEAN);

  return return_code;

}


/**
 * \brief Returns the codec tier info.
 */
wcd_result wcd_get_codec_tier_info(wcd_codec_info* p_codec_info)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 micbias3_anc_dmic34_disable;
  uint8 comp_rx4_tx34_disable;

  do
  {
    return_code = wcd_activate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error activating bus before resetting.>\n");
      break; //while loop
    }

    // read qfuse data registers to determine codec tier
    return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_QFUSE_DATA_OUT1_REG, 0xF, 0, (uint8*)&micbias3_anc_dmic34_disable);
    return_code = wcd_register_read(E_WCD_CODEC_MAIN, (uint16)WCD_CODEC_QFUSE_DATA_OUT2_REG, 0x3F, 0, (uint8*)&comp_rx4_tx34_disable);

    wcd_deactivate_bus();

    WCD_DEBUG_INFO_2("=WCD_93XX: Codec TIER info: micbias3_anc_dmic34_disable - 0x%x comp_rx4_tx34_disable - 0x%x\n",micbias3_anc_dmic34_disable, comp_rx4_tx34_disable);

    /* In case of wcd9302 Micbias3, ANC, DMIC3, DMIC4 are disabled along with
       RX4 and TX3, TX4 decimator path. If both QFUSE_DATA_OUT1 and QFUSE_DATA_OUT2
       register bits are set that indiates codec tier is WCD9302.
       Codec Info which was initially retrived in get_codec_info is updated here
       based on qfused features.
    */
    if(micbias3_anc_dmic34_disable == 0x0 && comp_rx4_tx34_disable == 0x0)
    {
      g_wcd_codec_info.codec = E_WCD_WCD9306;
      p_codec_info->codec = E_WCD_WCD9306;
      WCD_DEBUG_INFO_0("=WCD_93XX: Codec TIER info: WCD9306 DETECTED\n");
    }
    else if(0 != micbias3_anc_dmic34_disable || 0 != comp_rx4_tx34_disable)
    {
      g_wcd_codec_info.codec = E_WCD_WCD9306;
      p_codec_info->codec = E_WCD_WCD9306;
      WCD_DEBUG_INFO_0("=WCD_93XX: Codec TIER info: WCD9302 DETECTED\n");
    }
    else
    {
      /* This may happen if Codec is not appropriately fused */
      WCD_DEBUG_INFO_0("=WCD_93XX: Codec TIER info: Unsupported Codec Hw Detected\n");
    }

  } while(WCD_G_FALSE_BOOLEAN);

  return return_code;
}


/**
 * \brief Initialize max value for ADCs, DMICs, TX & RX path count based on codec.
 */
wcd_result wcd_initialize_codec_max_count(wcd_codec_info codec_info)
{
  wcd_result return_code = E_WCD_SUCCESS;

  // set micbias count based on codec
  WCDH_SET_MICBIAS_COUNT(codec_info.codec);

  if(codec_info.codec == E_WCD_WCD9306)
  {
    wcd_max_adcs = WCD_MAX_ADCS_WCD9306;
    wcd_max_dmics = WCD_MAX_DMICS_WCD9306;
    tx_path_count = WCD9306_TX_PATH_COUNT;
    rx_path_count = WCD9306_RX_PATH_COUNT;
  }
  else
  {
    wcd_max_adcs = WCD_MAX_ADCS_WCD9302;
    wcd_max_dmics = WCD_MAX_DMICS_WCD9302;
    tx_path_count = WCD9302_TX_PATH_COUNT;
    rx_path_count = WCD9302_RX_PATH_COUNT;
  }

  WCD_DEBUG_INFO_1("=WCD_93XX: <Initialized codec max counts: codec - 0x%x>\n",codec_info.codec);
  WCD_DEBUG_INFO_2("=WCD_93XX: <Initialized codec max counts: ADCs - 0x%x DMICs - 0x%x>\n",wcd_max_adcs, wcd_max_dmics);
  WCD_DEBUG_INFO_2("=WCD_93XX: <Initialized codec max counts: TX PATH - 0x%x RX PATH - 0x%x>\n",tx_path_count, rx_path_count);

  return return_code;
}


/**
 * \brief Initialize the decimator mapping for ADCS, DMICs & Slimbus Tx port based on codec.
 */
wcd_result wcd_initialize_codec_decimator_mapping(wcd_codec_info codec_info)
{
  wcd_result return_code = E_WCD_SUCCESS;

  if(codec_info.codec == E_WCD_WCD9306)
  {
    adc_sbtx_map = wcd9306_adc_sbtx_map;
    dmic_sbtx_map = wcd9306_dmic_sbtx_map;

    adc_1_txpath_map = wcd9306_adc_1_txpath_map;
    adc_2_txpath_map = wcd9306_adc_2_txpath_map;
    adc_3_txpath_map = wcd9306_adc_3_txpath_map;
    adc_4_txpath_map = wcd9306_adc_4_txpath_map;
    adc_5_txpath_map = wcd9306_adc_5_txpath_map;

    dmic_1_txpath_map = wcd9306_dmic_1_txpath_map;
    dmic_2_txpath_map = wcd9306_dmic_2_txpath_map;
    dmic_3_txpath_map = wcd9306_dmic_3_txpath_map;
    dmic_4_txpath_map = wcd9306_dmic_4_txpath_map;

    sbtx_1_txpath_map = wcd9306_sbtx_1_txpath_map;
    sbtx_2_txpath_map = wcd9306_sbtx_2_txpath_map;
    sbtx_3_txpath_map = wcd9306_sbtx_3_txpath_map;
    sbtx_4_txpath_map = wcd9306_sbtx_4_txpath_map;
    sbtx_5_txpath_map = wcd9306_sbtx_5_txpath_map;

    txpath_element_map = wcd9306_txpath_element_map;
    rxpath_element_map = wcd9306_rxpath_element_map;

    ADC_elements = wcd9306_ADC_elements;
    DMIC_elements = wcd9306_DMIC_elements;
  }
  else
  {
    adc_sbtx_map = wcd9302_adc_sbtx_map;
    dmic_sbtx_map = wcd9302_dmic_sbtx_map;

    adc_1_txpath_map = wcd9302_adc_1_txpath_map;
    adc_2_txpath_map = wcd9302_adc_2_txpath_map;
    adc_3_txpath_map = wcd9302_adc_3_txpath_map;
    adc_4_txpath_map = wcd9302_adc_4_txpath_map;
    adc_5_txpath_map = wcd9302_adc_5_txpath_map;

    dmic_1_txpath_map = wcd9302_dmic_1_txpath_map;
    dmic_2_txpath_map = wcd9302_dmic_2_txpath_map;
    dmic_3_txpath_map = wcd9302_dmic_3_txpath_map;
    dmic_4_txpath_map = wcd9302_dmic_4_txpath_map;

    sbtx_1_txpath_map = wcd9302_sbtx_1_txpath_map;
    sbtx_2_txpath_map = wcd9302_sbtx_2_txpath_map;
    sbtx_3_txpath_map = wcd9302_sbtx_3_txpath_map;
    sbtx_4_txpath_map = wcd9302_sbtx_4_txpath_map;
    sbtx_5_txpath_map = wcd9302_sbtx_5_txpath_map;

    txpath_element_map = wcd9302_txpath_element_map;
    rxpath_element_map = wcd9302_rxpath_element_map;

    ADC_elements = wcd9302_ADC_elements;
    DMIC_elements = wcd9302_DMIC_elements;

	WCD_DEBUG_INFO_0("=WCD_93XX: <Selecting DEC for WCD9302>\n");
  }

  WCD_DEBUG_INFO_1("=WCD_93XX: <Initialized codec decimator mapping: codec - 0x%x>\n",codec_info.codec);

  return return_code;
}

/**
 * \brief Initialize route stack based on codec.
 */
wcd_result wcd_initialize_route_stack(wcd_codec_info codec_info)
{
  wcd_result return_code = E_WCD_SUCCESS;

  if(codec_info.codec == E_WCD_WCD9306)
  {
    rs_sbrx_earout = wcd9306_rs_sbrx_earout_;
    rs_sbrx_earout_ultrasound = wcd9306_rs_sbrx_earout_ultrasound;
    rs_sbrx_hph_l = wcd9306_rs_sbrx_hph_l;
    rs_sbrx_hph_r = wcd9306_rs_sbrx_hph_r;
    rs_sbrx_lo1 = wcd9306_rs_sbrx_lo1;
    rs_sbrx_lo2 = wcd9306_rs_sbrx_lo2;
    rs_sbrx_spkr = wcd9306_rs_sbrx_spkr;
  }
  else
  {
    rs_sbrx_earout = wcd9302_rs_sbrx_earout;
    rs_sbrx_earout_ultrasound = wcd9302_rs_sbrx_earout_ultrasound;
    rs_sbrx_hph_l = wcd9302_rs_sbrx_hph_l;
    rs_sbrx_hph_r = wcd9302_rs_sbrx_hph_r;
    rs_sbrx_lo1 = wcd9302_rs_sbrx_lo1;
    rs_sbrx_lo2 = wcd9302_rs_sbrx_lo2;
    rs_sbrx_spkr = wcd9302_rs_sbrx_spkr;
  }

  WCD_DEBUG_INFO_1("=WCD_93XX: <Initialized codec route stack: codec - 0x%x>\n",codec_info.codec);

  return return_code;

}

/**
 * \brief Returns the maximum number of RX ports based on bus type.
 */
wcd_result wcd_get_max_rx_ports(uint32* port_count)
{
  if (port_count == NULL)
  {
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (wcd_init_options.bustype == E_WCD_I2C)
  {
    *port_count = WCD_MAX_I2S_RX_PORTS;
  }
  else
  {
    *port_count = WCD_MAX_RX_PORTS;
  }

  return E_WCD_SUCCESS;
}

/**
 * \brief Returns the maximum number of TX ports based on bus type.
 */
wcd_result wcd_get_max_tx_ports(uint32* port_count)
{
  if (port_count == NULL)
  {
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (wcd_init_options.bustype == E_WCD_I2C)
  {
    *port_count = WCD_MAX_I2S_TX_PORTS;
  }
  else
  {
    *port_count = WCD_MAX_TX_PORTS;
  }

  return E_WCD_SUCCESS;
}


/******************************************************************************
 **************                      SIDETONE                   ***************
 *****************************************************************************/

uint8 iir_num;

wcd_usecase *iir_one_rx_usecase = NULL;
wcd_usecase *iir_one_tx_usecase = NULL;
wcd_adie_codec_db_sidetone_iir_config iir_one_configuration;
#define RX1_B3_CTL__MIX2_INP1_SEL_IIR1 0x3

wcd_usecase *iir_two_rx_usecase = NULL;
wcd_usecase *iir_two_tx_usecase = NULL;
wcd_adie_codec_db_sidetone_iir_config iir_two_configuration;
#define RX1_B3_CTL__MIX2_INP1_SEL_IIR2 0x4

/**
 * \brief Returns the Sidetone RX usecase associated with the usecase if it is part of a sidetone
 * usecase. Returns NULL if its not part of a sidetone usecase
 */
wcd_usecase* wcd_check_for_sidetone_usecase(wcd_usecase *usecase)
{
  if (usecase == iir_one_rx_usecase || usecase == iir_one_tx_usecase)
  {
    return iir_one_rx_usecase;
  }

  if (usecase == iir_two_rx_usecase || usecase == iir_two_tx_usecase)
  {
    return iir_two_rx_usecase;
  }

  return NULL;
}

/**
 * \brief Sets the first or second IIR in codec to be used as the sidetone path for this particular usecase.
 */
wcd_result wcd_set_sidetone_iir(wcd_usecase *rx_usecase, wcd_usecase* tx_usecase)
{
  if (rx_usecase == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot set NULL usecase in sidetone.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (iir_one_rx_usecase == rx_usecase || iir_two_rx_usecase == rx_usecase)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <This usecase is already setup for sidetone.>\n");
    return E_WCD_SIDETONE_ALREADY_ENABLED;
  }

  if (iir_one_rx_usecase == NULL)
  {
    iir_one_rx_usecase = rx_usecase;
    iir_one_tx_usecase = tx_usecase;
    iir_num = 1;
    return E_WCD_SUCCESS;
  }
  else if (iir_two_rx_usecase == NULL)
  {
    iir_two_rx_usecase = rx_usecase;
    iir_two_tx_usecase = tx_usecase;
    iir_num = 2;
    return E_WCD_SUCCESS;
  }

  WCD_DEBUG_ERROR_0("=WCD_93XX: <Two sidetone paths already setup. No more IIRs available for sidetone.>\n");
  return E_WCD_REACHED_SIDETONE_LIMIT;
}

/**
 * \brief Returns the saved sidetone IIR configuration for hte particular usecase.
 */
wcd_result wcd_get_sidetone_iir_configuration(wcd_usecase *usecase, wcd_adie_codec_db_sidetone_iir_config* config)
{
  wcd_adie_codec_db_sidetone_iir_config *saved_config = NULL;
  uint32 iterator = 0;

  if (config == NULL || usecase == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <IIR Configuration parameter is NULL.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (usecase == iir_one_rx_usecase)
  {
    saved_config = &iir_one_configuration;
  }
  else if (usecase == iir_two_rx_usecase)
  {
    saved_config = &iir_two_configuration;
  }
  else
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <This usecase is not setup for sidetone currently.>\n");
    return E_WCD_SIDETONE_NOT_ENABLED;
  }

  config->enable = saved_config->enable;
  config->pregain = saved_config->pregain;

  for(iterator = 0; iterator < 25; iterator++)
  {
    config->coefficients[iterator] = saved_config->coefficients[iterator];
  }

  return E_WCD_SUCCESS;
}

/**
 * \brief Configures the IIR filter for sidetone on the codec, for the appropriate usecase
 */
wcd_result wcd_configure_sidetone_iir(wcd_usecase *usecase, wcd_adie_codec_db_sidetone_iir_config* config)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 gain = (uint8)config->pregain;

  uint32 iterator = 0;

  wcd_adie_codec_db_sidetone_iir_config *saved_config = NULL;

  uint16 pregain_reg = 0x0;
  uint16 coeff_ptr_reg = 0x0;
  uint16 coeff_val_reg = 0x0;

  uint16 stage_enable_reg = 0x0;

  if (usecase == iir_one_rx_usecase)
  {
    pregain_reg = 0x340;
    coeff_ptr_reg = 0x34A;
    coeff_val_reg = 0x34B;
    stage_enable_reg = 0x348;

    saved_config = &iir_one_configuration;
  }
  else if (usecase == iir_two_rx_usecase)
  {
    pregain_reg = 0x350;
    coeff_ptr_reg = 0x35A;
    coeff_val_reg = 0x35B;
    stage_enable_reg = 0x358;

    saved_config = &iir_two_configuration;
  }
  else
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <This usecase is not setup for sidetone currently.>\n");
    return E_WCD_SIDETONE_NOT_ENABLED;
  }

  //save the enable and pregain data in the saved configuration
  saved_config->enable = config->enable;
  saved_config->pregain = config->pregain;

  return_code = wcd_activate_bus();

  //set the pregain only if the config is enabled
  if (saved_config->enable)
  {
    //set pre gain
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, pregain_reg, 0xFF, 0, gain);
  }

  //write all coefficients
  for(iterator = 0; iterator < 25; iterator++)
  {
    //write coefficients if only the config is enabled
    if (saved_config->enable)
    {
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, coeff_ptr_reg, 0x7F, 0, (uint8)(iterator << 2));

      return_code = wcd_register_write(E_WCD_CODEC_MAIN, coeff_val_reg, 0xFF, 0, (uint8)((config->coefficients[iterator] & 0xFF)));
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, coeff_val_reg, 0xFF, 0, (uint8)((config->coefficients[iterator] & 0xFF00) >> 8));
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, coeff_val_reg, 0xFF, 0, (uint8)((config->coefficients[iterator] & 0xFF0000) >> 16));
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, coeff_val_reg, 0xFF, 0, (uint8)((config->coefficients[iterator] & 0xFF000000) >> 24));
    }

    //save coefficients to the saved configuration
    saved_config->coefficients[iterator] = config->coefficients[iterator];
  }

  //enable all 5 stages
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, stage_enable_reg, 0x1F, 0, 0x1F);

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  return_code = wcd_deactivate_bus();

  return return_code;
}

/**
 * \brief Enables sidetone and connects the sidetone mux to the decimator passed in the parameter
 */
wcd_result wcd_enable_sidetone(wcd_usecase* usecase, wcd_element* tx_path)
{
  uint16 reg = 0x0;

  uint8 mask = 0x1F;
  uint8 enable_mask = 0x0;

  uint8 shift = 0;
  uint8 val = 0;

  wcd_result return_code = E_WCD_SUCCESS;

  if (iir_one_rx_usecase == usecase)
  {
    reg = 0x397;
    enable_mask = 0x1;
  }
  else if (iir_two_rx_usecase == usecase)
  {
    reg = 0x39B;
    enable_mask = 0x2;
  }
  else
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <This usecase is not setup for sidetone currently.>\n");
    return_code = E_WCD_SIDETONE_NOT_ENABLED;
  }

  switch (tx_path->id)
  {
    case WCD_TX_PATH_1_ID:
      val = 0x1;
      break;
    case WCD_TX_PATH_2_ID:
      val = 0x2;
      break;
    case WCD_TX_PATH_3_ID:
      val = 0x3;
      break;
    case WCD_TX_PATH_4_ID:
      val = 0x4;
      break;
    default:
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

  if (return_code == E_WCD_SUCCESS)
  {
    return_code = wcd_activate_bus();

    return_code = wcd_register_write(E_WCD_CODEC_MAIN, reg, mask, shift, val);

    //#CDC_CLK_SD_CTL__IIR1_CLK_EN = ENABLE
    //0x313; 0x01; 0x01
    //#CDC_CLK_SD_CTL__IIR2_CLK_EN = ENABLE
    //0x313; 0x02; 0x02
    return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x313, enable_mask, 0, enable_mask);

    wcd_bus_flush(E_WCD_CODEC_MAIN);

    return_code = wcd_deactivate_bus();
  }

  return return_code;
}

/**
 * \brief * \brief Disables sidetone if possible.
 * Also clears the sidetone from the IIR being used.
 */
wcd_result wcd_disable_sidetone(wcd_usecase* usecase)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 enable_mask = 0x0;
  uint8 reset_mask = 0x0;
  uint16 stage_enable_reg = 0x0;

  if (iir_one_rx_usecase == usecase)
  {
    enable_mask = 0x1;
    reset_mask = 0x10;
    stage_enable_reg = 0x348;
    iir_one_rx_usecase = NULL;
    iir_one_tx_usecase = NULL;
  }
  else if (iir_two_rx_usecase == usecase)
  {
    enable_mask = 0x2;
    reset_mask = 0x20;
    stage_enable_reg = 0x358;
    iir_two_rx_usecase = NULL;
    iir_two_tx_usecase = NULL;
  }
  else
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <This usecase is not setup for sidetone currently.>\n");
    return E_WCD_SIDETONE_NOT_ENABLED;
  }

  return_code = wcd_activate_bus();

  //disable stages
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, stage_enable_reg, 0x1F, 0, 0x0);

  //#CDC_CLK_SD_CTL__IIR1_CLK_EN = DISABLE
  //0x313; 0x01; 0x00
  //#CDC_CLK_SD_CTL__IIR2_CLK_EN = DISABLE
  //0x313; 0x02; 0x00
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x313, enable_mask, 0, 0x00);

  //reset IIR
  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x313, reset_mask, 0, reset_mask);

  wcd_bus_flush(E_WCD_CODEC_MAIN);
  WCD_SLEEP(5);

  return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x313, reset_mask, 0, 0x00);
  wcd_bus_flush(E_WCD_CODEC_MAIN);

  return_code = wcd_deactivate_bus();

  return return_code;
}

/**
 * \brief Connects sidetone to the RX Path.
 */
wcd_result wcd_connect_sidetone(wcd_element* rx_path)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 val = 0;

  return_code = wcd_activate_bus();

  switch (iir_num){
    case 1:
      val = RX1_B3_CTL__MIX2_INP1_SEL_IIR1;
      break;
    case 2:
      val = RX1_B3_CTL__MIX2_INP1_SEL_IIR2;
      break;
    default:
      break;
  }

  switch(rx_path->id)
  {
    case WCD_RX_1_PATH_ID:
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x382, 0x07, 0, val);
      break;
    case WCD_RX_2_PATH_ID:
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x385, 0x07, 0, val);
      break;
    case WCD_RX_4_PATH_ID:
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x38A, 0x07, 0, val);
      break;
  }

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  return_code = wcd_deactivate_bus();

  return return_code;
}

/**
 * \brief Disconnects sidetone to the RX Path.
 */
wcd_result wcd_disconnect_sidetone(wcd_element* rx_path)
{
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_activate_bus();

  switch(rx_path->id)
  {
    case WCD_RX_1_PATH_ID:
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x382, 0x07, 0, 0x0);
      break;
    case WCD_RX_2_PATH_ID:
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x385, 0x07, 0, 0x0);
      break;
    case WCD_RX_4_PATH_ID:
      return_code = wcd_register_write(E_WCD_CODEC_MAIN, 0x38A, 0x07, 0, 0x0);
      break;
  }

  wcd_bus_flush(E_WCD_CODEC_MAIN);

  return_code = wcd_deactivate_bus();

  return return_code;
}

/******************************************************************************
 **************                 SPEAKER PROTECTION              ***************
 *****************************************************************************/

wcd_result mad_configure_txfe(wcd_configure_mad* config, wcd_bool enable_MAD_mode )
{
  HANDLE_UNUSED_PARAMETER(config);
  HANDLE_UNUSED_PARAMETER(enable_MAD_mode);
  return E_WCD_SUCCESS;
}

/**
 * \brief Sets the port number of the voltage in feedback speaker protection
 */
wcd_result wcd_get_spkr_prot_voltage_port(uint32* port_num)
{
  HANDLE_UNUSED_PARAMETER(port_num);
  return E_WCD_SUCCESS;
}

/**
 * \brief Sets the port number of the current in feedback speaker protection
 */
wcd_result wcd_get_spkr_prot_current_port(uint32* port_num)
{
  HANDLE_UNUSED_PARAMETER(port_num);
  return E_WCD_SUCCESS;
}

/**
 * \brief Enables speaker protection on the codec, if supported
 */
wcd_result wcd_enable_speaker_protection(void)
{
  return E_WCD_SUCCESS;
}

/**
 * \brief Disables speaker protection on the codec, if supported
 */
wcd_result wcd_disable_speaker_protection(void)
{
  return E_WCD_SUCCESS;
}

/******************************************************************************
 **************                 MAD                             ***************
 *****************************************************************************/

wcd_result wcd_configure_mad_audio_beacon( wcd_audio_beacon_config* config, uint8 offset )
{
  HANDLE_UNUSED_PARAMETER(config);
  HANDLE_UNUSED_PARAMETER(offset);
  return E_WCD_SUCCESS;
}


wcd_result wcd_configure_mad_ultrasound( wcd_ultrasound_config* config, uint8 offset )
{
  HANDLE_UNUSED_PARAMETER(config);
  HANDLE_UNUSED_PARAMETER(offset);
  return E_WCD_SUCCESS;
}


wcd_result wcd_configure_mad_common( wcd_configure_mad* p_command )
{
  HANDLE_UNUSED_PARAMETER(p_command);
  return E_WCD_SUCCESS;
}

wcd_result wcd_disable_mad_common(void )
{
  return E_WCD_SUCCESS;
}

wcd_bool wcd_audio_configured( void )
{
  return (usecase_enabled_count != 0);
}

/******************************************************************************
 **************                 Common                          ***************
 *****************************************************************************/
/**
 * \brief Save vdd buck voltage locally, passed by client during wcd_init
 */
wcd_result wcd_store_codec_buck_voltage(wcd_vdd_buck_voltage buck_voltage)
{
  wcd_codec_buck_voltage = buck_voltage;

  return E_WCD_SUCCESS;
}

/**
 * \brief return vdd buck voltage which is set
 */
wcd_vdd_buck_voltage wcd_get_codec_buck_voltage(void)
{
  return wcd_codec_buck_voltage;
}

/**
 * \brief Get connection direction from element type
 */
wcd_result wcd_get_connection_direction(wcd_element* source_element, wcd_element* sink_element, adie_codec_direction *connection_direction)
{
  if ((source_element->type == WCD_ADC_TYPE_ID || source_element->type == WCD_DMIC_TYPE_ID ||
      source_element->type == WCD_ANC_CHANNEL_TYPE_ID) && sink_element->type == WCD_BUS_PORT_TYPE_ID)
  {
    *connection_direction = ADIE_CODEC_TX;
  }
  else
  {
    *connection_direction = ADIE_CODEC_RX;
  }

  return E_WCD_SUCCESS;
}

#ifdef WCD_MBHC_ENABLE

/**
 * \brief Function to call the client callback to toggle the 
 *        external codec reset GPIO
 */
wcd_result wcd_reset_codec_external_pin(void)
{
  wcd_result rc = E_WCD_SUCCESS;
  int gpio_value;

  gpio_value = E_WCD_GPIO_HIGH;
  rc  = WCDSwitchGpio(NULL, E_WCD_GPIO_RESET, &gpio_value);
  WCD_SLEEP(5);
  gpio_value = E_WCD_GPIO_LOW;
  rc |= WCDSwitchGpio(NULL, E_WCD_GPIO_RESET, &gpio_value);
  WCD_SLEEP(5);
  gpio_value = E_WCD_GPIO_HIGH;
  rc |= WCDSwitchGpio(NULL, E_WCD_GPIO_RESET, &gpio_value);
  WCD_SLEEP(5);
  if (rc != E_WCD_SUCCESS)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Error toggling the external reset pin.>\n");
  }

  return rc;
}

/**
 * \brief Function to write value to an O/P GPIO
 */
wcd_result WCDSwitchGpio(void *param, wcd_module_gpio_id id, int *value)
{
  wcd_result rc = E_WCD_SUCCESS;
  wcd_gpio_intf_payload gpio_payload;

  gpio_payload.gpio_owner           = E_WCD_CORE;
  gpio_payload.gpio_id.wcd_gpio_id 	= id;
  gpio_payload.type                 = (wcd_gpio_type) E_WCD_GPIO_OUTPUT;
  gpio_payload.value                = value;

  if (wcd_gpio_cb_data.cb != NULL)
  {
    rc = (wcd_result)(wcd_gpio_cb_data.cb((void *)&gpio_payload, (unsigned int)(sizeof(gpio_payload)), (void *)(wcd_gpio_cb_data.cb_data)));
    if (rc != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <WCDSwitchGpio: GPIO=(%d) failure> \n", id);
      rc = E_WCD_GENERIC_ERROR;
    }
    else
    {
      WCD_DEBUG_INFO_2("=WCD_93XX: <WCDSwitchGpio: GPIO=(%d), Value=(%d) successful> \n", id, *value);
      rc = E_WCD_SUCCESS;
    }
  }
  else
  {
    WCD_DEBUG_ERROR_1("=WCD_93XX: <WCDSwitchGpio: NULL GPIO Intf CB for GPIO=(%d)> \n", id);
    rc = E_WCD_GENERIC_WARNING;
  }

  HANDLE_UNUSED_PARAMETER(param);

  return rc;
}

#endif
