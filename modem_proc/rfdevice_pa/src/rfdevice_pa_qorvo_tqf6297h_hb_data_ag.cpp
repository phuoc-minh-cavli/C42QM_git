
/*
WARNING: This file is auto-generated.

Generated using: pa_autogen.pl
Generated from:  4.0.79 of RFDevice_PA.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2013-2023 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //components/rel/rfdevice_pa.mpss/1.30/src/rfdevice_pa_qorvo_tqf6297h_hb_data_ag.cpp#2 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_pa_qorvo_tqf6297h_hb_data_ag.h"


#define RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS 5

#define RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_BIAS_NUM_REGS 1
static uint8 rfdevice_pa_qorvo_tqf6297h_hb_pa_set_bias_regs[RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_BIAS_NUM_REGS] =  {0x01, };
static int16 rfdevice_pa_qorvo_tqf6297h_hb_pa_set_bias_data[RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS][4][RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_BIAS_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    { 0xF4, },  /* PA Range: 0 */
    { 0xF4, },  /* PA Range: 1 */
    { 0xF9, },  /* PA Range: 2 */
    { 0x43, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 1 */
    { 0xC3, },  /* PA Range: 0 */
    { 0xC3, },  /* PA Range: 1 */
    { 0x9B, },  /* PA Range: 2 */
    { 0x42, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 2 */
    { 0x9F, },  /* PA Range: 0 */
    { 0x9F, },  /* PA Range: 1 */
    { 0xF9, },  /* PA Range: 2 */
    { 0x53, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 3 */
    { 0xB4, },  /* PA Range: 0 */
    { 0xB4, },  /* PA Range: 1 */
    { 0xA9, },  /* PA Range: 2 */
    { 0x32, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 4 */
    { 0xBF, },  /* PA Range: 0 */
    { 0xBF, },  /* PA Range: 1 */
    { 0x9D, },  /* PA Range: 2 */
    { 0x53, },  /* PA Range: 3 */
  },
};


#define RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_RANGE_NUM_REGS 1
static uint8 rfdevice_pa_qorvo_tqf6297h_hb_pa_set_range_regs[RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_RANGE_NUM_REGS] =  {0x00, };
static int16 rfdevice_pa_qorvo_tqf6297h_hb_pa_set_range_data[RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS][4][RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_RANGE_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x26, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 1 */
    { 0x14, },  /* PA Range: 0 */
    { 0x14, },  /* PA Range: 1 */
    { 0x16, },  /* PA Range: 2 */
    { 0x16, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 2 */
    { 0x14, },  /* PA Range: 0 */
    { 0x14, },  /* PA Range: 1 */
    { 0x16, },  /* PA Range: 2 */
    { 0x16, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 3 */
    { 0x0C, },  /* PA Range: 0 */
    { 0x0C, },  /* PA Range: 1 */
    { 0x0E, },  /* PA Range: 2 */
    { 0x0E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 4 */
    { 0x1C, },  /* PA Range: 0 */
    { 0x1C, },  /* PA Range: 1 */
    { 0x1E, },  /* PA Range: 2 */
    { 0x1E, },  /* PA Range: 3 */
  },
};


#define RFDEVICE_PA_QORVO_TQF6297H_HB_PA_ON_NUM_REGS 1
static uint8 rfdevice_pa_qorvo_tqf6297h_hb_pa_on_regs[RFDEVICE_PA_QORVO_TQF6297H_HB_PA_ON_NUM_REGS] =  {RFFE_INVALID_REG_ADDR /*Warning: Not Specified*/, };
static int16 rfdevice_pa_qorvo_tqf6297h_hb_pa_on_data[RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS][RFDEVICE_PA_QORVO_TQF6297H_HB_PA_ON_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 1 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 2 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 3 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 4 */
    RF_REG_INVALID, 
  },
};


#define RFDEVICE_PA_QORVO_TQF6297H_HB_PA_OFF_NUM_REGS 1
static uint8 rfdevice_pa_qorvo_tqf6297h_hb_pa_off_regs[RFDEVICE_PA_QORVO_TQF6297H_HB_PA_OFF_NUM_REGS] =  {0x00, };
static int16 rfdevice_pa_qorvo_tqf6297h_hb_pa_off_data[RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS][RFDEVICE_PA_QORVO_TQF6297H_HB_PA_OFF_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    0x00, 
  },
  { /* PORT NUM: 1 */
    0x00, 
  },
  { /* PORT NUM: 2 */
    0x00, 
  },
  { /* PORT NUM: 3 */
    0x00, 
  },
  { /* PORT NUM: 4 */
    0x00, 
  },
};


#define RFDEVICE_PA_QORVO_TQF6297H_HB_PA_TRIGGER_NUM_REGS 1
static uint8 rfdevice_pa_qorvo_tqf6297h_hb_pa_trigger_regs[RFDEVICE_PA_QORVO_TQF6297H_HB_PA_TRIGGER_NUM_REGS] =  {0x1C, };
static int16 rfdevice_pa_qorvo_tqf6297h_hb_pa_trigger_data[RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS][RFDEVICE_PA_QORVO_TQF6297H_HB_PA_TRIGGER_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    0x07, 
  },
  { /* PORT NUM: 1 */
    0x07, 
  },
  { /* PORT NUM: 2 */
    0x07, 
  },
  { /* PORT NUM: 3 */
    0x07, 
  },
  { /* PORT NUM: 4 */
    0x07, 
  },
};

/* singleton instance ptr */
rfdevice_pa_data *rfdevice_pa_qorvo_tqf6297h_hb_data_ag::rfdevice_pa_qorvo_tqf6297h_hb_data_ptr = NULL;

rfdevice_pa_data * rfdevice_pa_qorvo_tqf6297h_hb_data_ag::get_instance()
{
  if (rfdevice_pa_qorvo_tqf6297h_hb_data_ptr == NULL)
  {
    rfdevice_pa_qorvo_tqf6297h_hb_data_ptr = (rfdevice_pa_data *)new rfdevice_pa_qorvo_tqf6297h_hb_data_ag();
  }
  return( (rfdevice_pa_data *)rfdevice_pa_qorvo_tqf6297h_hb_data_ptr);
}

//constructor
rfdevice_pa_qorvo_tqf6297h_hb_data_ag::rfdevice_pa_qorvo_tqf6297h_hb_data_ag()
  :rfdevice_pa_data()
{
}

//destructor
rfdevice_pa_qorvo_tqf6297h_hb_data_ag::~rfdevice_pa_qorvo_tqf6297h_hb_data_ag()
{
  rfdevice_pa_qorvo_tqf6297h_hb_data_ptr = NULL;
}

boolean rfdevice_pa_qorvo_tqf6297h_hb_data_ag::settings_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                                          rfdevice_pa_reg_settings_type *settings)
{
  boolean ret_val = FALSE;

  if (NULL == settings || NULL == cfg)
  {
    return FALSE;
  }

  if (cfg->port >= RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS )
  {
    settings->addr = NULL;
    settings->data = NULL;
    settings->num_regs = 0;
    return FALSE;
  }

  if ( (cfg->req == RFDEVICE_PA_SET_BIAS_DATA) && (cfg->pa_range < 4) )
  {
    settings->addr = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_set_bias_regs[0]);
    settings->data = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_set_bias_data[cfg->port][cfg->pa_range][0]);
    settings->num_regs = RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_BIAS_NUM_REGS;
    settings->bias_reg_idx = 0;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_SET_RANGE_DATA) && (cfg->pa_range < 4) )
  {
    settings->addr = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_set_range_regs[0]);
    settings->data = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_set_range_data[cfg->port][cfg->pa_range][0]);
    settings->num_regs = RFDEVICE_PA_QORVO_TQF6297H_HB_PA_SET_RANGE_NUM_REGS;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_ON_DATA) )
  {
    settings->addr = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_on_regs[0]);
    settings->data = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_on_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_PA_QORVO_TQF6297H_HB_PA_ON_NUM_REGS;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_OFF_DATA) )
  {
    settings->addr = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_off_regs[0]);
    settings->data = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_off_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_PA_QORVO_TQF6297H_HB_PA_OFF_NUM_REGS;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_TRIGGER_DATA) )
  {
    settings->addr = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_trigger_regs[0]);
    settings->data = &(rfdevice_pa_qorvo_tqf6297h_hb_pa_trigger_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_PA_QORVO_TQF6297H_HB_PA_TRIGGER_NUM_REGS;
    ret_val = TRUE;
  }

  return ret_val;
}

boolean rfdevice_pa_qorvo_tqf6297h_hb_data_ag::sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                                          rfdevice_pa_cmd_seq_type *cmd_seq)
{
  boolean ret_val = FALSE;

  if (NULL == cmd_seq || NULL == cfg)
  {
    return FALSE;
  }

  else if (RFDEVICE_PA_RESET_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_PA_THERM_ON_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_PA_THERM_READ_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_PA_THERM_OFF_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
  }

  return ret_val;
}

boolean rfdevice_pa_qorvo_tqf6297h_hb_data_ag::device_info_get( rfdevice_pa_info_type *pa_info )
{
  boolean ret_val = FALSE;

  if ( NULL == pa_info )
  {
    return FALSE;
  }
  else
  {
    pa_info->mfg_id = 0x286;
    pa_info->prd_id = 0x1C;
    pa_info->prd_rev = 1;
    pa_info->num_ports = RFDEVICE_PA_QORVO_TQF6297H_HB_NUM_PORTS;
    pa_info->num_pa_ranges = 4;
    pa_info->lpm_pm_trig_flag = FALSE;  
    ret_val = TRUE;
  }
  return ret_val;
}

boolean rfdevice_pa_qorvo_tqf6297h_hb_data_ag::get_temperature_lut(int16 **lut_addr, uint8 *lut_size)
{
   boolean ret_val = FALSE;   

   if (lut_addr != NULL)  
   {
      *lut_addr = NULL; 
      *lut_size = -1;
      ret_val = TRUE;
   }

   else 
   {
      ret_val = FALSE;
   }

   return ret_val;
}

