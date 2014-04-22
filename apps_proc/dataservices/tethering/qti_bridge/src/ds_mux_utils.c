/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   U T I L S

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed 
  for decoding and encoding MUX packets.

  DS MUX UTILS function could be invoked independently to generate
  mux frame from DSM items  visa-versa

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_utils.c#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "ds_mux_utils.h"
#include "ds_mux.h"
/*===========================================================================
                      LOCAL  VARIABLE DEFINATION
===========================================================================*/

/* CRC table to be used for FCS calculation 
 * Inserted from 3GPP Spec 27.010 version 5
 */
static uint8  ds_mux_util_crc_table[DS_MUX_UTIL_CRC_TABLE_ELEMENTS] = 
{   /* reversed, 8-bit, poly=0x07 */
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,  
    0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,  
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,  
    0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,  
    0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,

    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,  
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,  
    0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,  
    0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,  
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,  
    0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,  
    0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,  
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,  
    0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,

    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,  
    0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,  
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,  
    0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,  
    0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

/*===========================================================================
                      LOCAL FUNCTION DECLARATIONS
===========================================================================*/

ds_mux_frame_type_enum_type ds_mux_util_get_frame_type_from_ctrl_octet
(
    uint8 ctrl_octet
);

ds_mux_result_enum_type ds_mux_util_stop_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_flag_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_addr_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_ctrl_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_len_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_info_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_fcs_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
);

ds_mux_result_enum_type ds_mux_util_generate_control_field
(
  uint8*                      control_fld_ptr,
  ds_mux_frame_type_enum_type frame_type,
  boolean                     pf_bit
);

/*===========================================================================
                       FUNCTION DEFINATION
===========================================================================*/


ds_mux_frame_type_enum_type ds_mux_util_get_frame_type_from_ctrl_octet
(
    uint8 ctrl_octet
)
{
  ds_mux_frame_type_enum_type ret_frame_type = DS_MUX_FRAME_TYPE_INVALID_FRAME;
  uint8                       ctrl_octet_wo_pf = ctrl_octet & (~DS_MUX_UTIL_CTRL_OCTET_PF_MASK);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-----------------------------------------------------------------------
    Convert CTRL Command into FRAME type
  -----------------------------------------------------------------------*/ 
  switch(ctrl_octet_wo_pf)
  {
    case DS_MUX_UTIL_CTRL_OCTET_SABM_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_SABM;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_UA_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_UA;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_DM_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_DM;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_DISC_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_DISC;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_UIH_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_UIH;
      break;
    }
    case DS_MUX_UTIL_CTRL_OCTET_UI_CODE_WO_PF:
    {
      ret_frame_type = DS_MUX_FRAME_TYPE_UTIL_UI;
      break;
    }
    default:
    {
      break;
    }
  }

  return ret_frame_type;

}/* ds_mux_util_get_frame_type_from_ctrl_octet */

ds_mux_io_frame_struct_type* ds_mux_util_alloc_frame
(

)
{
  ds_mux_io_frame_struct_type*  frame_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  frame_ptr = (ds_mux_io_frame_struct_type*)malloc(sizeof(ds_mux_io_frame_struct_type));
  if ( NULL == frame_ptr )
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_util_decode_input(): Failed: memory alloc for frame ");
    return NULL;
  }
  
  memset( frame_ptr, 0x0, sizeof(ds_mux_io_frame_struct_type) );
  return frame_ptr;

}/* ds_mux_util_alloc_frame */

ds_mux_result_enum_type ds_mux_util_stop_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS; 
  byte                            curr_char           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    1. Pull out one character from DSM item
    2. Move into START PARSED only when we receive F9 flag
       2.1 Otherthen F9 flag, Any char will be ignored and continue to search F9.
    Note: DSM PULLUP API will free the DSM item if used become 0
  -----------------------------------------------------------------------*/ 
  if ( 1 == dsm_pullup_long ( item_ptr, &curr_char, 1) )
  {
    if( DS_MUX_UTIL_START_STOP_FLAG_CODE == curr_char )
    {
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_START_PARSED;
    }
    else
    {
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_STOP_PARSED;
    }
  }
  else
  {
    DS_MUX_LOG_MSG_ERROR_1("ds_mux_util_stop_parsed(): Unable to retrieve flag byte  %d", curr_char);
    result = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_util_addr_parsed */

ds_mux_result_enum_type ds_mux_utils_calc_fcs
(
  dsm_item_type                 *item_ptr,
  uint16                         total_pkt_len,
  uint8                          curr_fcs,
  uint8                         *fcs_field
)
{
  uint8                           temp_fcs      = curr_fcs;
  uint8                           octet_buf     = 0;
  uint16                          peek_offset   = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    1. Start FCS calculation over whole buffer
    2. Calculate one's complement 
  -----------------------------------------------------------------------*/ 

  /* Step1 */
  while (total_pkt_len--)
  {
    if (FALSE ==  dsm_peek_byte(item_ptr, peek_offset++, &octet_buf))
    {
      return DS_MUX_FAILURE;
    }
    temp_fcs = ds_mux_util_crc_table[temp_fcs ^ octet_buf];
  }
  /* Step2 */
  temp_fcs = DS_MUX_UTIL_FCS_INIT_VAL - temp_fcs;
  
  *fcs_field = temp_fcs;

  return DS_MUX_SUCCESS;
}/* ds_mux_utils_calc_fcs */

uint8 ds_mux_utils_calc_fcs_per_char
(
  uint8                          octet_buf,
  uint8                          curr_fcs
)
{
  uint8                           temp_fcs = curr_fcs;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  temp_fcs = ds_mux_util_crc_table[curr_fcs ^ octet_buf];
  
  return temp_fcs;
}

ds_mux_result_enum_type ds_mux_util_flag_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS; 
  byte                            curr_char           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-----------------------------------------------------------------------
     1. Pull out one character from DSM item
     2. Stay into START PARSED  when we receive F9 flag again
     3. Generate DLCI ID
     4. EA bit should set to 1 as we are suppoting MAX DLCI ID till 63 only
     5. Calculate C/R bit. Initiator should set command bit to 1
     6. Generate DLCI ID from remaining 6 bits( remove EA and CR bit by rightshift)
     7. Move into Next step
     8. Calculate FCS
     Note: DSM PULLUP API will free the DSM item if used become 0
   -----------------------------------------------------------------------*/ 
  if ( 1 == dsm_pullup_long ( item_ptr, &curr_char, 1) )
  {
    /*Step 2*/
    if( DS_MUX_UTIL_START_STOP_FLAG_CODE == curr_char )
    {
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_START_PARSED;
    }
    else
    {
      /*Step 4*/
      if ( 0 == (curr_char & DS_MUX_UTIL_OCTET_EA_MASK))
      {
         LOG_MSG_ERROR_0("ds_mux_util_decode_input(): EA bit is not set in address ");
         result   = DS_MUX_FAILURE;
      }
      else
      {
        /*Step 5*/
        rx_info->curr_frame_ptr->frame.command_response = ( curr_char & DS_MUX_UTIL_ADDR_OCTET_CR_MASK ) ? TRUE : FALSE;
        /*Step 6*/
        rx_info->curr_frame_ptr->frame.dlci = (curr_char >> 2);
        /*Step 7*/
        rx_info->curr_frame_state = DS_MUX_IO_FRAME_ADDR_PARSED;
         /*Step 8*/
        rx_info->curr_frame_ptr->frame.fcs = ds_mux_utils_calc_fcs_per_char ( curr_char, DS_MUX_UTIL_FCS_INIT_VAL);
      }
    }
  }
  else
  {
    result = DS_MUX_FAILURE;
  }
  
  return result;
}/* ds_mux_util_flag_parsed */

ds_mux_result_enum_type ds_mux_util_addr_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS; 
  byte                            curr_char           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
     1. Pull out one character from DSM item
     2. Get Frame type from control octet
     3. Calculate poll/final bit
     4. Move into Next step
     5. Calculate FCS
     Note: DSM PULLUP API will free the DSM item if used become 0
   -----------------------------------------------------------------------*/ 
  if ( 1 == dsm_pullup_long ( item_ptr, &curr_char, 1) )
  {
    /*Step 2*/
    rx_info->curr_frame_ptr->frame.frame_type = 
      ds_mux_util_get_frame_type_from_ctrl_octet(curr_char);
    
    if ( DS_MUX_FRAME_TYPE_INVALID_FRAME == rx_info->curr_frame_ptr->frame.frame_type)
    {
      DS_MUX_LOG_MSG_ERROR_0("ds_mux_util_addr_parsed(): Invalid frame type ");
      result = DS_MUX_FAILURE;
    }
    else
    {
      /*Step 3*/
      rx_info->curr_frame_ptr->frame.poll_final = ( curr_char & DS_MUX_UTIL_CTRL_OCTET_PF_MASK ) ? TRUE : FALSE;
      //Store Ctrl byte
      rx_info->curr_frame_ptr->frame.control_field = curr_char;
      /*Step 4*/
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_CTRL_PARSED;
      /*Step 5*/
      rx_info->curr_frame_ptr->frame.fcs = ds_mux_utils_calc_fcs_per_char ( 
                                             curr_char, rx_info->curr_frame_ptr->frame.fcs );
    }
  }
  else
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_util_addr_parsed(): Unable to retrieve ctrl byte  ");
    result = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_util_addr_parsed */


ds_mux_result_enum_type ds_mux_util_ctrl_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS; 
  byte                            curr_char           = 0;
  byte                            ext_len_char        = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-----------------------------------------------------------------------
     1. Pull out one character from DSM item
     2. Length can be in 1 or 2 byte based on EA bit
        2.1 if EA bit is 1 then length is in 1 byte
        2.2 if EA bit is 0 thne length is in 2 byte
     3. Calculate length from one or two byte
     4. Move into Next step
     5. Calculate FCS on length bytes
     Note: DSM PULLUP API will free the DSM item if used become 0
   -----------------------------------------------------------------------*/ 
  if ( 1 == dsm_pullup_long ( item_ptr, &curr_char, 1) )
  {
    /*Step 2*/
    if ( 1 == (curr_char & DS_MUX_UTIL_OCTET_EA_MASK))
    {
      /*Step 2.1*/
      rx_info->curr_frame_ptr->frame.length = (uint16)(curr_char >> 1);
      /*Step 5*/
      rx_info->curr_frame_ptr->frame.fcs = ds_mux_utils_calc_fcs_per_char ( 
                                             curr_char, rx_info->curr_frame_ptr->frame.fcs );
	  rx_info->curr_frame_state = DS_MUX_IO_FRAME_LEN_PARSED;

    }
    else
    {
      /*Step 2.2*/
      if (1 == dsm_pullup_long ( item_ptr, &ext_len_char, 1))
      {
        rx_info->curr_frame_ptr->frame.length = (uint16)(ext_len_char);
        rx_info->curr_frame_ptr->frame.length <<= 7;
        rx_info->curr_frame_ptr->frame.length |= (uint16)(curr_char >> 1);
        /*Step 4*/
        rx_info->curr_frame_state = DS_MUX_IO_FRAME_LEN_PARSED;
        /* Check against MAX length */
        if ( rx_info->curr_frame_ptr->frame.length > DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN )
        {
          result  = DS_MUX_FAILURE;
        }
        /*Step 5*/
        rx_info->curr_frame_ptr->frame.fcs = ds_mux_utils_calc_fcs_per_char ( 
                                               curr_char, rx_info->curr_frame_ptr->frame.fcs );
        //Calculate fcs on second length byte
        rx_info->curr_frame_ptr->frame.fcs = ds_mux_utils_calc_fcs_per_char ( 
                                               ext_len_char, rx_info->curr_frame_ptr->frame.fcs );
      }
      else
      {
        DS_MUX_LOG_MSG_ERROR_0("ds_mux_util_ctrl_parsed(): Invalid extended length ");
        result  = DS_MUX_FAILURE;
      }
    }
  }
  else
  {
    result = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_util_ctrl_parsed */


ds_mux_result_enum_type ds_mux_util_len_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result             = DS_MUX_SUCCESS; 
  uint32                          rem_info_len       = 0;
  uint32                          curr_info_len      = 0;
  dsm_item_type                  *tail_item_ptr      = NULL;
  uint8                           temp_fcs           = 0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {

    //Info field is not present (e.g commands)
    if ( 0 == rx_info->curr_frame_ptr->frame.length )
    {
      //commands doesnt have information field
      rx_info->curr_frame_ptr->frame.information_ptr = NULL;
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_INFO_PARSED;
      break;
    }
    //There could be three cases :
    // 1  Whole information present in current DSM chain
    // 2. Current DSM has partial info and next DSM iteam could have remaining info
    // 3. Current frame already have partial info field and we are returing here for remaining info
    if ( NULL != rx_info->curr_frame_ptr->frame.information_ptr )
    {
      curr_info_len = dsm_length_packet( rx_info->curr_frame_ptr->frame.information_ptr );
      
      if ( curr_info_len < rx_info->curr_frame_ptr->frame.length )
      {
        rem_info_len = rx_info->curr_frame_ptr->frame.length - curr_info_len;
        // Split the packet
        dsm_split_packet( item_ptr, &tail_item_ptr, rem_info_len, DSM_DS_LARGE_ITEM_POOL );
        // Append into previous info prt
        dsm_append( &rx_info->curr_frame_ptr->frame.information_ptr, item_ptr);
        //Update Item ptr to remaining chain
        *item_ptr = tail_item_ptr;
      }
      else
      {
        //Invalid state of len
        result  = DS_MUX_FAILURE; 
        DS_MUX_LOG_MSG_ERROR_2("ds_mux_util_len_parsed(): Invalid curr pkt len %d  pkt info len %d", 
          curr_info_len, rx_info->curr_frame_ptr->frame.length);
      }
      break;
    }

    // Split the packet till end of the information

    dsm_split_packet( item_ptr, &tail_item_ptr, 
      rx_info->curr_frame_ptr->frame.length, DSM_DS_LARGE_ITEM_POOL );

    rx_info->curr_frame_ptr->frame.information_ptr = *item_ptr;
        
    //Update the item pointer
    *item_ptr = tail_item_ptr;
    
    break;
  }while(0);
  if ( NULL != rx_info->curr_frame_ptr->frame.information_ptr)
  {
    curr_info_len = dsm_length_packet( rx_info->curr_frame_ptr->frame.information_ptr );
  }
  //Complete info got parsed
  if ( curr_info_len == rx_info->curr_frame_ptr->frame.length)
  {
    rx_info->curr_frame_state = DS_MUX_IO_FRAME_INFO_PARSED;
    
    //We got the complete infromation hence we can do check sum on complete packet
    //Calculate FCS on info (Applicable for UIH frame only)
    
    if ( 0 != curr_info_len && DS_MUX_FRAME_TYPE_UTIL_UI == rx_info->curr_frame_ptr->frame.frame_type )
    {
      result = ds_mux_utils_calc_fcs( 
              rx_info->curr_frame_ptr->frame.information_ptr , 
              curr_info_len,
              rx_info->curr_frame_ptr->frame.fcs , 
              &temp_fcs);

      rx_info->curr_frame_ptr->frame.fcs = temp_fcs;
      
    }
    else
    {
      //as we are not calculating fcs on info, we should take one's complement of existing fcs
      /* One's complement */
      rx_info->curr_frame_ptr->frame.fcs = DS_MUX_UTIL_FCS_INIT_VAL - rx_info->curr_frame_ptr->frame.fcs;
    }
    
  }
  else if ( curr_info_len < rx_info->curr_frame_ptr->frame.length)
  { // wait for next DSM item to be parsed for remaining info
    rx_info->curr_frame_state = DS_MUX_IO_FRAME_LEN_PARSED;
  }
  else
  {
    DS_MUX_LOG_MSG_ERROR_2("ds_mux_util_len_parsed(): Invalid curr pkt len %d  pkt info len %d", 
      curr_info_len, rx_info->curr_frame_ptr->frame.length);
    result  = DS_MUX_FAILURE; 
  }
  
  return result;
}/* ds_mux_util_len_parsed */

ds_mux_result_enum_type ds_mux_util_info_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS; 
  byte                            curr_char           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
     1. Pull out one character from DSM item
     2. Get FCS value from FCS octet
     3. Compare FCS calculate on the frame with the FCS mentioned in the frame
     Note: DSM PULLUP API will free the DSM item if used become 0
   -----------------------------------------------------------------------*/ 
  if ( 1 == dsm_pullup_long ( item_ptr, &curr_char, 1) )
  {
    if ( curr_char != rx_info->curr_frame_ptr->frame.fcs )
    {
      //Check Sum failed
      result = DS_MUX_FAILURE;
    }
    rx_info->curr_frame_state = DS_MUX_IO_FRAME_FCS_PARSED;
  }
  else
  {
    result = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_util_info_parsed */

ds_mux_result_enum_type ds_mux_util_fcs_parsed
(
  dsm_item_type                 **item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS; 
  byte                            curr_char           = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
     1. Pull out one character from DSM item
     2. Verify end flag (f9)
     3. Once END FLAG is verified store current pased frame into queue and set 
        current frame pointer to NULL
     Note: Frame memory will be freed after processing in DS_MUX_IO
     Note: DSM PULLUP API will free the DSM item if used become 0
   -----------------------------------------------------------------------*/ 
  if ( 1 == dsm_pullup_long ( item_ptr, &curr_char, 1) )
  {
    if( DS_MUX_UTIL_START_STOP_FLAG_CODE == curr_char )
    {
      rx_info->curr_frame_state = DS_MUX_IO_FRAME_STOP_PARSED;
      /*Step 3*/
      q_put( &rx_info->decoded_frame_q, &rx_info->curr_frame_ptr->link );
      /*Step 4*/
      rx_info->curr_frame_ptr = NULL;
    }
    else
    {
      result = DS_MUX_FAILURE;
    }
  }
  else
  {
    result = DS_MUX_FAILURE;
  }
  return result;
}/* ds_mux_util_info_parsed */


ds_mux_result_enum_type ds_mux_util_decode_input
(
  dsm_item_type                  *item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  boolean                         processing_frames   = TRUE;
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( NULL == item_ptr ) || ( NULL == rx_info ) )
  {
    DS_MUX_LOG_MSG_ERROR_2("ds_mux_util_decode_input(): Invalid params dsm item 0x%x  rx info 0x%x", 
        item_ptr, rx_info);
    return DS_MUX_INVALID_PARAM;
  }

  /*-----------------------------------------------------------------------
     1. Loop till no data left in DSM item or processing aborted due to error or processing is complete
   -----------------------------------------------------------------------*/
  while( (TRUE == processing_frames) && ( NULL != item_ptr) )
  {
    //Check if current frame is NULL, Allocate the frame
    if( ( NULL == rx_info->curr_frame_ptr) && 
        ( NULL == ( rx_info->curr_frame_ptr = ds_mux_util_alloc_frame())))
    {
      processing_frames = FALSE;
      /*-----------------------------------------------------------------------
         Abroting remaining parsing. Freeing DSM item.Status remain un changed
       -----------------------------------------------------------------------*/
      dsm_free_packet( &item_ptr);
      break;
    }
    
    switch ( rx_info->curr_frame_state )
    {
      case DS_MUX_IO_FRAME_STOP_PARSED:
      case DS_MUX_IO_FRAME_INIT:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_stop_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_START_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_flag_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_ADDR_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_addr_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_CTRL_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_ctrl_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
        break;

      case DS_MUX_IO_FRAME_LEN_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_len_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
      break;

      case DS_MUX_IO_FRAME_INFO_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_info_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
      break;

      case DS_MUX_IO_FRAME_FCS_PARSED:
        if ( DS_MUX_FAILURE == (result = ds_mux_util_fcs_parsed( &item_ptr, rx_info)))
        {
          processing_frames   = FALSE;
        }
      break;
      default :
      {
      }
      break;
    }/* End of switch case */
  }/* End of while loop*/
  //In case of error:: Reset the current frame state and  free the current frame 
  if ( DS_MUX_FAILURE == result)
  {
    rx_info->curr_frame_state = DS_MUX_IO_FRAME_INIT;
    
    if ( NULL != rx_info->curr_frame_ptr )
    {
      dsm_free_packet( &rx_info->curr_frame_ptr->frame.information_ptr);
      rx_info->curr_frame_ptr->frame.information_ptr = NULL;
      DS_MUX_MEM_FREE(rx_info->curr_frame_ptr);
      rx_info->curr_frame_ptr   = NULL;
    }
    
    dsm_free_packet( &item_ptr);
  }

  return result;
}/* ds_mux_util_decode_input */

ds_mux_result_enum_type ds_mux_util_process_power_save_flag
(
  dsm_item_type                  *item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  uint32                          pkt_len             = 0;
  uint32                          cnt                 = 0;
  uint8                           flag_byte           = 0;
  boolean                         wake_up_flag_found  = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( NULL == item_ptr ) || ( NULL == rx_info ) )
  {
    DS_MUX_LOG_MSG_ERROR_2("ds_mux_util_decode_input(): Invalid params dsm item 0x%x  rx info 0x%x", 
        item_ptr, rx_info);
    return DS_MUX_INVALID_PARAM;
  }

  pkt_len = dsm_length_packet( item_ptr );

  if ( 0 != pkt_len )
  {
    for ( cnt = 0; cnt < pkt_len; cnt++)
    {
      if ( TRUE  == dsm_peek_byte(item_ptr, cnt, &flag_byte) )
      {
        if (DS_MUX_UTIL_START_STOP_FLAG_CODE == flag_byte)
        {
          wake_up_flag_found = TRUE;
          break;
        }
      }
    }
  }
  /*-----------------------------------------------------------------------
     Send same wake up sequnce to host
   -----------------------------------------------------------------------*/
  if (TRUE  == wake_up_flag_found)
  {
    result = ds_mux_phys_transmit( item_ptr );
    
    if ( DS_MUX_FAILURE == result)
    {
      dsm_free_packet( &item_ptr );
    }
    else
    {
      ds_mux_set_mux_state(DS_MUX_STATE_WAKEUP_FLAG_SENT);
    }
  }
  
  return result;
  
}/* ds_mux_util_process_power_save_flag */

ds_mux_result_enum_type ds_mux_util_process_frame_after_wake_up
(
  dsm_item_type                  *item_ptr,
  ds_mux_io_rx_info              *rx_info
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  uint32                          pkt_len             = 0;
  uint32                          cnt                 = 0;
  uint8                           flag_byte           = 0;
  boolean                         valid_frame_found   = FALSE;
  uint8                           dlci_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( ( NULL == item_ptr ) || ( NULL == rx_info ) )
  {
    DS_MUX_LOG_MSG_ERROR_2("ds_mux_util_decode_input(): Invalid params dsm item 0x%x  rx info 0x%x", 
        item_ptr, rx_info);
    return DS_MUX_INVALID_PARAM;
  }

  pkt_len = dsm_length_packet( item_ptr );

  if ( 0 != pkt_len )
  {
    for ( cnt = 0; cnt < pkt_len; cnt++)
    {
      if ( TRUE  == dsm_peek_byte(item_ptr, cnt, &flag_byte) )
      {
        if (DS_MUX_UTIL_START_STOP_FLAG_CODE != flag_byte)
        {
          valid_frame_found = TRUE;
          break;
        }
      }
    }
  }
  /*-----------------------------------------------------------------------
     Found character other then F9, Try to decode the frame. If it is valid frame then 
     come out from powersave
   -----------------------------------------------------------------------*/
  if (TRUE == valid_frame_found)
  {
    
    result = ds_mux_util_decode_input( item_ptr, rx_info);

    if ( DS_MUX_SUCCESS == result)
    {
      ds_mux_set_mux_state(DS_MUX_STATE_OPENED);
    
      for(dlci_id = DS_MUX_DLCI_ID_0;  dlci_id <= DS_MUX_MAX_LOGICAL_CONNECTION; dlci_id++  )
      {
        ds_mux_enable_flow_mask( dlci_id, DS_MUX_FLOW_DISABLE_PSC );
        
      }
    }
    else
    {
      DS_MUX_LOG_MSG_ERROR_0("ds_mux_util_decode_input(): DS MUX is expecting valid frame ");
      ASSERT(0);
    }
  }
  else //send wake up sync
  {
    result = ds_mux_phys_transmit( item_ptr );
    
    if ( DS_MUX_FAILURE == result)
    {
      dsm_free_packet( &item_ptr );
    }
  }
  return result;
}/* ds_mux_util_process_power_save_flag */

ds_mux_result_enum_type ds_mux_util_generate_control_field
(
  uint8*                      control_fld_ptr,
  ds_mux_frame_type_enum_type frame_type,
  boolean                     pf_bit
)
{
  ds_mux_result_enum_type     result    = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(frame_type)
  {
    case DS_MUX_FRAME_TYPE_UTIL_SABM:
    {
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_SABM_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_UA:
    {
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_UA_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_DM:
    {
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_DM_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_DISC:
    {
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_DISC_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_UIH:
    {
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_UIH_CODE_WO_PF ;
       break;
    }
    case DS_MUX_FRAME_TYPE_UTIL_UI:
    {
      *control_fld_ptr = DS_MUX_UTIL_CTRL_OCTET_UI_CODE_WO_PF ;
      break;
    }
    default:
    {
      result    = DS_MUX_FAILURE;
      break;
    }
  }

  if ( DS_MUX_SUCCESS == result && 1 == pf_bit)
  {
    *control_fld_ptr |= DS_MUX_UTIL_CTRL_OCTET_PF_MASK;
  }

  return result;
} /* ds_mux_util_generate_control_field() */

ds_mux_result_enum_type ds_mux_util_insert_len 
(
  dsm_item_type            **item_ptr,
  uint16                     length
)
{
  uint8                           len_field     = 0;
  uint8                           len_sec_octet = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( length <= 127 )
  {
    // Insert EA bit 
    len_field   = length << 1;
    len_field  |= DS_MUX_UTIL_OCTET_EA_MASK;
  
    //Insert length byte at back
    if ( 1 != dsm_pushdown_packed
        ( item_ptr, &len_field, 1, DSM_DS_SMALL_ITEM_POOL))
    {
      return DS_MUX_FAILURE;
    }
    //Calculate FCS here
  }
  else if (  length <= DS_MUX_UTIL_BASIC_FRAME_MAX_INFO_LEN )
  {
    len_sec_octet = (uint8)(length >> 7);

    len_field     = (uint8)((length << 1) & (0x00fe));
    
    if ( 1 != dsm_pushdown_packed
      ( item_ptr, &len_sec_octet, 1, DSM_DS_SMALL_ITEM_POOL))
    {
      return DS_MUX_FAILURE;
    }
    if ( 1 != dsm_pushdown_packed
        ( item_ptr, &len_field, 1, DSM_DS_SMALL_ITEM_POOL))
    {
      return DS_MUX_FAILURE;
    }
    
  }
  else
  {
    DS_MUX_LOG_MSG_ERROR_1("ds_mux_util_insert_len(): Invalid len %d", length);
    return DS_MUX_FAILURE;
  }
  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_len*/

ds_mux_result_enum_type ds_mux_util_insert_ctl
(
  dsm_item_type                **item_ptr,
  ds_mux_frame_type_enum_type  frame_type,
  boolean                      poll_final
)
{
  uint8                         ctl_field        = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //Generate control field
  if ( DS_MUX_SUCCESS != ds_mux_util_generate_control_field( &ctl_field, 
        frame_type, poll_final))
  {
    return DS_MUX_FAILURE;
  }
  
  /*-----------------------------------------------------------------------
    Insert Control  byte at back
  -----------------------------------------------------------------------*/
  if ( 1 != dsm_pushdown_packed
       (item_ptr, &ctl_field, 1, DSM_DS_SMALL_ITEM_POOL))
  {
    return DS_MUX_FAILURE;
  }
  
  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_ctl*/

ds_mux_result_enum_type ds_mux_util_insert_addr
(
  dsm_item_type            **item_ptr,
  uint8                    dlci,
  boolean                  command_response

)
{
  uint8                           addr_field       = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (IS_DLCI_INVALID(dlci))
  {
    return  DS_MUX_FAILURE;
  }
  
  addr_field = dlci << 2;

  if ( TRUE == command_response)
  {
    addr_field |= DS_MUX_UTIL_ADDR_OCTET_CR_MASK;
  }

  addr_field |= DS_MUX_UTIL_OCTET_EA_MASK;
  /*-----------------------------------------------------------------------
    Insert Address byte at back
  -----------------------------------------------------------------------*/
  if ( 1 != dsm_pushdown_packed
       (item_ptr, &addr_field, 1, DSM_DS_SMALL_ITEM_POOL))
  {
    return DS_MUX_FAILURE;
  }

  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_addr*/

ds_mux_result_enum_type ds_mux_util_generate_fcs
(
  dsm_item_type                 *item_ptr,
  ds_mux_frame_type_enum_type    frame_type,
  uint16                         frame_info_len,
  uint8                         *fcs_field
)
{
  uint16                          total_pkt_len = 2;// Initialized with 2 (Address + ctrl byte)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // Calculate bytes for length
  if ( frame_info_len <= 127)
  {
    //Length is 1 byte long
    total_pkt_len += 1;
  }
  else
  { 
    //Length is two byte long
    total_pkt_len += 2;
  }
 
  /*-----------------------------------------------------------------------
    In case of UI frame include info length as well
  -----------------------------------------------------------------------*/ 
  if( DS_MUX_FRAME_TYPE_UTIL_UI == frame_type )
  {
    total_pkt_len += frame_info_len;
  }

  /*-----------------------------------------------------------------------
    Verify:: DSM item should have enough data
  -----------------------------------------------------------------------*/  
  if ( (dsm_length_packet(item_ptr)) < (uint32)( total_pkt_len) )
  {
    DS_MUX_LOG_MSG_ERROR_2("ds_mux_util_generate_fcs(): invalid len %d pkt len %d", 
        total_pkt_len, dsm_length_packet(item_ptr));
    
    return DS_MUX_FAILURE;
  }

  if ( 0 != total_pkt_len &&  DS_MUX_FAILURE ==  ds_mux_utils_calc_fcs( 
              item_ptr, total_pkt_len, DS_MUX_UTIL_FCS_INIT_VAL, fcs_field) )
  {
    return DS_MUX_FAILURE;
  }

  return DS_MUX_SUCCESS;
}/* ds_mux_util_generate_fcs */

ds_mux_result_enum_type ds_mux_util_insert_fcs
(
  dsm_item_type                 **item_ptr,
  ds_mux_frame_type_enum_type     frame_type,
  uint16                          frame_info_len
)
{
  uint8                           fcs_field        = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( DS_MUX_SUCCESS != ds_mux_util_generate_fcs( *item_ptr, frame_type, frame_info_len, &fcs_field) )
  {
    return DS_MUX_FAILURE;
  }
  
  /*-----------------------------------------------------------------------
    Insert FCS byte byte at back
  -----------------------------------------------------------------------*/
  if ( 1 !=  dsm_pushdown_tail
               (item_ptr, &fcs_field, 1, DSM_DS_SMALL_ITEM_POOL))
  {
    return DS_MUX_FAILURE;
  }

  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_fcs*/

ds_mux_result_enum_type ds_mux_util_insert_start_n_stop
(
  dsm_item_type            **item_ptr
)
{
  uint8              start_stop_field = DS_MUX_UTIL_START_STOP_FLAG_CODE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Inseart start flag
  -----------------------------------------------------------------------*/ 
  if ( 1 != dsm_pushdown_packed
       (item_ptr, &start_stop_field, 1, DSM_DS_SMALL_ITEM_POOL))
  {
    return DS_MUX_FAILURE;
  }
  /*-----------------------------------------------------------------------
    Inseart end flag
  -----------------------------------------------------------------------*/ 
  if ( 1 !=  dsm_pushdown_tail
               (item_ptr, &start_stop_field, 1, DSM_DS_SMALL_ITEM_POOL))
  {
    return DS_MUX_FAILURE;
  }

  return  DS_MUX_SUCCESS;
}/* ds_mux_util_insert_start_n_stop*/

ds_mux_result_enum_type ds_mux_util_encode_cmd_input
( 
  dsm_item_type            **item_ptr,
  ds_mux_io_frame_type     *frame
)
{
  ds_mux_result_enum_type         result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == item_ptr || NULL == frame)
    {
      result = DS_MUX_FAILURE;
      break;
    }
    /*-----------------------------------------------------------------------
      Order of API calls should be retained 
    -----------------------------------------------------------------------*/ 
    if ( DS_MUX_FAILURE == ds_mux_util_insert_len ( item_ptr, frame->length))
      DS_MUX_SET_ERROR_AND_BREAK(result)
        
    if ( DS_MUX_FAILURE == ds_mux_util_insert_ctl ( item_ptr, frame->frame_type, frame->poll_final))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( DS_MUX_FAILURE == ds_mux_util_insert_addr ( item_ptr, frame->dlci, frame->command_response))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( DS_MUX_FAILURE == ds_mux_util_insert_fcs ( item_ptr, frame->frame_type, frame->length ))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    if ( DS_MUX_FAILURE == ds_mux_util_insert_start_n_stop ( item_ptr))
      DS_MUX_SET_ERROR_AND_BREAK(result)

    //TBD: read spec and Add extra F9 at the end for making len multiple of 4 
    
  }while( 0 );

  return result;
}/* ds_mux_util_encode_cmd_input */

ds_mux_result_enum_type ds_mux_util_encode_data_input
( 
  dsm_item_type            **item_ptr,
  ds_mux_io_frame_type     *frame
)
{
  ds_mux_result_enum_type         result              = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == item_ptr || NULL == *item_ptr || NULL == frame)
    {
      result = DS_MUX_FAILURE;
      break;
    }
    /*-----------------------------------------------------------------------
      Order of API calls should be retained 
    -----------------------------------------------------------------------*/ 
    ds_mux_util_encode_cmd_input( item_ptr, frame);

  
  }while( 0 );

  return result;
}/* ds_mux_util_encode_data_input */

ds_mux_result_enum_type ds_mux_util_encode_msg_input
( 
  dsm_item_type            **item_ptr,
  ds_mux_io_frame_type     *frame
)
{
  ds_mux_result_enum_type         result           = DS_MUX_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if(NULL == item_ptr || NULL == *item_ptr || NULL == frame)
    {
      result = DS_MUX_FAILURE;
      break;
    }
    /*-----------------------------------------------------------------------
      Order of API calls should be retained 
    -----------------------------------------------------------------------*/ 
    result = ds_mux_util_encode_cmd_input( item_ptr, frame);
 }while( 0 );

  return result;
}/* ds_mux_util_encode_msg_input*/


void ds_mux_util_prepare_cmd_frame
(
  uint8                       dlci,
  ds_mux_frame_type_enum_type frame_type,
  boolean                     is_command,
  boolean                     is_final,
  ds_mux_io_frame_type       *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(output_frame, 0x0, sizeof(ds_mux_io_frame_type));
  
  output_frame->dlci             = dlci;
  output_frame->frame_type       = frame_type;
  
  if ( TRUE == is_command )
  {
    output_frame->command_response = (TRUE == ds_mux_is_initiator()) ? TRUE : FALSE;
  }
  else
  {
    //Incase of response; Responder will set bit 1
    output_frame->command_response = (TRUE == ds_mux_is_initiator()) ? FALSE : TRUE;
  }
  if (TRUE == is_final)
  {
    output_frame->poll_final       = TRUE;
  }
  else
  {
    output_frame->poll_final       = FALSE;
  }
}/* ds_mux_util_prepare_cmd_frame */


void ds_mux_util_prepare_data_frame
(
  uint8                       dlci,
  ds_mux_frame_type_enum_type frame_type,
  ds_mux_io_frame_type       *output_frame
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(output_frame, 0x0, sizeof(ds_mux_io_frame_type));
  
  output_frame->dlci             = dlci;
  output_frame->frame_type       = frame_type;
  output_frame->command_response = FALSE;
  output_frame->poll_final       = FALSE;

}/* ds_mux_util_prepare_data_frame */

ds_mux_frame_type_enum_type ds_mux_util_convert_frame_type
( 
  ds_mux_subset_enum_type cmux_frame_type
)
{
  ds_mux_frame_type_enum_type return_frame;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( cmux_frame_type )
  {
    case DS_MUX_SUBSET_UIH:
      return_frame = DS_MUX_FRAME_TYPE_UTIL_UIH;
      break;

    case DS_MUX_SUBSET_UI:
      return_frame = DS_MUX_FRAME_TYPE_UTIL_UI;
      break;

    default:
     return_frame = DS_MUX_FRAME_TYPE_INVALID_FRAME;
     break;
  }
  return return_frame;
  
}/* ds_mux_util_convert_frame_type */


