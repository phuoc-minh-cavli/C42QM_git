/*====================================================================
 *
 * FILE: flash_otp_config.c
 *
 * SERVICES: Functions and data for Flash OTP Configurations
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash OTP config data.
 *
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 *          Copyright � 2008-2009,2011 Qualcomm Technologies Incorporated.
 *          All Rights Reserved.
 *          QUALCOMM Proprietary/GTDR
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/core.tx/6.0/storage/flash/src/dal/flash_otp_config.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 05/20/11     bb      Move otp configuration to a single common file
 * 03/23/09     eo      Fix OTP configuration indexing.
 * 11/15/08     mm      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_otp_config.h"

#define NUM_ARRAY_ELEMENTS(array, type)  (sizeof(array)/sizeof(type))
#define UUID_LEN_MAX 0x10

/* 
 * DISCLAIMER: "As these OTP command sequences and parameters are subject 
 *              to special vendor licensing agreements, OEMs need to 
 *              appropriately work with the memory vendors to include 
 *              the correct OTP command sequences and parameters. 
 *              Qualcomm does not guarantee these sequences and parameters 
 *              will work ".
 */

struct nand_bus_cycle cfg1_sequence[] = 
{ 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xFF,                   /* cmd */
    0x1,                    /* num address cycles */
    {
	  NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0x30,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0x65,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0x0,                    /* cmd */
    0x5,                    /* num address cycles */
    {
      NAND_ADDR_PAGE_NUMBER,  /* Address */
      {0x0},                    /* Page Number */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  }, 
  {
    NAND_CMD_PAIRED,        /* cmd group */
    0x30,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_READ,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  },  
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xFF,                   /* cmd */
    0x1,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  }   
};

struct nand_bus_cycle cfg2_sequence[] = 
{ 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xFF,                   /* cmd */
    0x1,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
	},
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0x5A,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  },  
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xB5,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0x00,                   /* cmd */
    0x5,                    /* num address cycles */
    {
      NAND_ADDR_PAGE_NUMBER,  /* Address */
      {0x0},                    /* Page Number */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  }, 
  {
    NAND_CMD_PAIRED,        /* cmd group */
    0x30,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_READ,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  },
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xFF,                   /* cmd */
    0x1,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  } 
};

struct nand_bus_cycle cfg3_sequence[] = 
{ 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xFF,                   /* cmd */
    0x1,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xAF,                   /* cmd */
    0x5,                    /* num address cycles */
    {
      NAND_ADDR_PAGE_NUMBER,  /* Address */
      {0x0},                    /* Page Number */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_NONE    /* ready/busy wait */
  },  
  {
    NAND_CMD_PAIRED,        /* cmd group */
    0x30,                   /* cmd */
    0x0,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},	                /* INVALID */
    },
    NAND_DATA_READ,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  }, 
  {
    NAND_CMD_SINGLE,        /* cmd group */
    0xFF,                   /* cmd */
    0x1,                    /* num address cycles */
    {
      NAND_ADDR_NONE,         /* Address */
      {0x0},                    /* INVALID */
    },
    NAND_DATA_NONE,         /* data direction */
    NAND_WAIT_ON_Rb_HIGH    /* ready/busy wait */
  }  
};

struct nand_bus_cycle cfg4_sequence[] = 
{
  {
    NAND_CMD_SINGLE,                /* cmd group */
    0x0 ,                           /* cmd */
    0x5,                            /* num address cycles */
    {
      NAND_ADDR_PAGE_NUMBER,          /* Address */
      {0x3C},                           /* Page Number */
    },
    NAND_DATA_NONE,                 /* data direction */
    NAND_WAIT_ON_Rb_NONE            /* ready/busy wait */
  },          
};

struct nand_bus_cycle cfg5_sequence[] = 
{
  {
    NAND_CMD_SINGLE,                /* cmd group */
    0xED,                           /* cmd */
    0x1,                            /* num address cycles */
    { 
      NAND_ADDR_BYTE_ADDRESS,         /* Address */
      {0x0},                            /* INVALID */
    },
    NAND_DATA_READ,                 /* data direction */
    NAND_WAIT_ON_Rb_NONE            /* ready/busy wait */
  }
};

/* UUID Params */

/* CFG1, CFG2 & CFG4 */
static struct nand_uuid_params cfg1_2_4_uuid_params =
{
  UUID_LEN_MAX,           /* UUID Length */
  0x2,                    /* Number of signature bytes */
  {0x55, 0xAA}            /* Signature bytes */
};

/* CFG3 & CFG5 */
static struct nand_uuid_params cfg3_5_uuid_params =
{
  UUID_LEN_MAX,           /* UUID Length */
  0,                      /* Number of signature bytes */
  {0xFF}                  /* Signature bytes */
};


/* OTP Config data for different devices */
static struct otp_config otp_config_data[] =
{
  {
    NUM_ARRAY_ELEMENTS(cfg1_sequence, struct nand_bus_cycle), 
    cfg1_sequence,    
    &cfg1_2_4_uuid_params 
  },
  {
    NUM_ARRAY_ELEMENTS(cfg2_sequence, struct nand_bus_cycle), 
    cfg2_sequence,
    &cfg1_2_4_uuid_params 
   },
  {
    NUM_ARRAY_ELEMENTS(cfg3_sequence, struct nand_bus_cycle), 
    cfg3_sequence,
    &cfg3_5_uuid_params 
  },
  {
    NUM_ARRAY_ELEMENTS(cfg4_sequence, struct nand_bus_cycle), 
    cfg4_sequence,
    &cfg1_2_4_uuid_params 
  },
  {
    NUM_ARRAY_ELEMENTS(cfg5_sequence, struct nand_bus_cycle), 
    cfg5_sequence,
    &cfg3_5_uuid_params 
  }           
};

/*
 * Gets the configuration based on the device type and maker id.
 */
void flash_get_otp_configs (enum nand_otp_config_type otp_sequence_cfg, 
  struct otp_config **cfg_data)
{
  if(otp_sequence_cfg < FLASH_NAND_OTP_SEQUENCE_UNKNOWN)
  {
    *cfg_data = &otp_config_data[otp_sequence_cfg]; 
  }
  else
  {
    *cfg_data = NULL;
  }
}
