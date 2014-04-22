/*=============================================================================
  @file sns_registry_util.c

  Utility functions pertinent to registry for use by Sensor Developers.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_registry_util.h"
#include "sns_registry.pb.h"
#include "sns_math_util.h"
#include "sns_pb_util.h"
#include "sns_types.h"

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

#define SNS_REGISTRY_MAX_FLT_ARR_LEN 20

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

/**Callback function used to decode sns_registry_read_event 
 * Function will extract each item within the read event
 * including items within subgroups and will optionally run a 
 * parsing function on the item passed.
 *  
 * @param[i] stream, Refer pb_callback_s::decode 
 * @param[i] field, Refer pb_callback_s::decode  
 * @param[i] arg of type sns_registry_decode_arg
 */
SNS_SECTION(".text.sns") bool
sns_registry_item_decode_cb(pb_istream_t *stream,
                            const pb_field_t *field,
                            void **arg)
{
  UNUSED_VAR(field);
  pb_istream_t stream_cpy = *stream;
  bool rv = true;

  sns_registry_decode_arg *decode_arg = (sns_registry_decode_arg *)*arg;
  pb_buffer_arg  *item_group_name = decode_arg->item_group_name;

  sns_registry_data_item reg_item = sns_registry_data_item_init_default;
  pb_buffer_arg  item_name;
  pb_buffer_arg  item_str_val;

  reg_item.name.funcs.decode = &pb_decode_string_cb;
  reg_item.name.arg = &item_name;

  reg_item.str.funcs.decode = &pb_decode_string_cb;
  reg_item.str.arg = &item_str_val;

  if(!pb_decode(stream, sns_registry_data_item_fields, &reg_item))
  {
    rv = false;
  }
  else if(0 == item_name.buf_len)
  {
    rv = false;
  }
  else
  {
    if(0 != reg_item.has_version)
    {
      decode_arg->version = reg_item.version;
    }
    if(0 != reg_item.has_subgroup)
    {
      sns_registry_data_item sub_item = sns_registry_data_item_init_default;
      pb_buffer_arg sub_group_name = {0,0};
      sns_registry_decode_arg sub_arg;

      sns_memscpy(&sub_arg, sizeof(sub_arg), decode_arg, sizeof(*decode_arg));
      sub_arg.item_group_name = &sub_group_name; 

      sub_item.name.funcs.decode = &pb_decode_string_cb;
      sub_item.name.arg = &sub_group_name;

      sub_item.subgroup.items.funcs.decode = &sns_registry_item_decode_cb;
      sub_item.subgroup.items.arg = &sub_arg;

      if(!pb_decode(&stream_cpy, sns_registry_data_item_fields, &sub_item))
      {
        rv = false;
      }
    }
    else
    {
      for(uint8_t i = 0; i < decode_arg->parse_info_len; i++)
      {
        uint8_t group_name_len =
          strlen(decode_arg->parse_info[i].group_name) + 1;

        if(group_name_len <= item_group_name->buf_len)
        {
          // supports partial match of parsed group name with 
          // the latter part of the item_group_name
          // for example "bias" match with "xxx.fac_cal.bias"
          uint8_t offset = item_group_name->buf_len - group_name_len;

          if(0 == strncmp((char *)item_group_name->buf + offset,
                          decode_arg->parse_info[i].group_name,
                          group_name_len))
          {
            rv = decode_arg->parse_info[i].parse_func(
              &reg_item,
              &item_name,
              &item_str_val,
              decode_arg->parse_info[i].parsed_buffer);

            break;
          }
        }
      }
    }
  }

  return rv;
}

/**Function used to parse items belonging to physical sensor 
 * configuration registry group
 *  
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item 
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_phy_sensor_cfg 
 *       where parsed items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_phy_sensor_cfg(sns_registry_data_item *reg_item,
                                  pb_buffer_arg  *item_name,
                                  pb_buffer_arg  *item_str_val,
                                  void *parsed_buffer)
{
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_sint)
  {
    sns_registry_phy_sensor_cfg *cfg =
      (sns_registry_phy_sensor_cfg *)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "is_dri", item_name->buf_len))
    {
      cfg->is_dri = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "hw_id",
                         item_name->buf_len))
    {
      cfg->hw_id = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "res_idx",
                         item_name->buf_len))
    {
      cfg->res_idx = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "sync_stream",
                         item_name->buf_len))
    {
      cfg->sync_stream = (reg_item->sint == 1) ?
        true : false;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**Function used to parse items belonging to physical sensor 
 * platform configuration registry group
 *  
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item 
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type 
 *       sns_registry_phy_sensor_pf_cfg where parsed items 
 *       are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_phy_sensor_pf_cfg(sns_registry_data_item *reg_item,
                                     pb_buffer_arg  *item_name,
                                     pb_buffer_arg  *item_str_val,
                                     void *parsed_buffer)
{
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else
  {
    sns_registry_phy_sensor_pf_cfg *cfg =
      (sns_registry_phy_sensor_pf_cfg *)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "bus_type", item_name->buf_len) &&
       reg_item->has_sint)
    {
      cfg->bus_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "bus_instance", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->bus_instance = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "slave_config", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->slave_config = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "i3c_address", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->i3c_address = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "min_odr", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->min_odr = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "max_odr", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->max_odr = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "min_bus_speed_khz", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->min_bus_speed_khz = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "max_bus_speed_khz", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->max_bus_speed_khz = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "reg_addr_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->reg_addr_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "dri_irq_num", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->dri_irq_num = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_pull_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_pull_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_is_chip_pin", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_is_chip_pin = (1 == reg_item->sint);
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_drive_strength", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_drive_strength = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "irq_trigger_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->irq_trigger_type = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "num_rail", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->num_rail = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "rail_on_state", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->rail_on_state = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "vddio_rail", item_name->buf_len) &&
            item_str_val->buf_len > 0)
    {
      sns_strlcpy(cfg->vddio_rail,
                  item_str_val->buf,
                  sizeof(cfg->vddio_rail));
    }
    else if(0 == strncmp((char*)item_name->buf, "vdd_rail", item_name->buf_len) &&
            item_str_val->buf_len > 0)
    {
      sns_strlcpy(cfg->vdd_rail,
                  item_str_val->buf,
                  sizeof(cfg->vdd_rail));
    }
    else if(0 == strncmp((char*)item_name->buf, "rigid_body_type", item_name->buf_len) &&
            reg_item->has_sint)
    {
      cfg->rigid_body_type = reg_item->sint;
    }
  }

  return rv;
}

/**Function used to parse items belonging to physical sensor 
 * platform configuration registry group
 *  
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item 
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type 
 *       sns_registry_phy_sensor_pf_cfg where parsed items 
 *       are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_axis_orientation(sns_registry_data_item *reg_item,
                                    pb_buffer_arg  *item_name,
                                    pb_buffer_arg  *item_str_val,
                                    void *parsed_buffer)
{
  bool rv = true;

  if(NULL == parsed_buffer || NULL == reg_item || NULL == item_name || 
     NULL == item_str_val)
  {
    rv = false;
  }
  else
  {
    triaxis_conversion *axis_map = (triaxis_conversion *)parsed_buffer;

    if(('x' == ((char*)item_name->buf)[0] || 'y' == ((char*)item_name->buf)[0] ||
        'z' == ((char*)item_name->buf)[0]) && item_str_val->buf_len > 0)
    {
      triaxis axis_idx = TRIAXIS_X;

      if('y' == ((char*)item_name->buf)[0])
      {
        axis_idx = TRIAXIS_Y;
      }
      else if('z' == ((char*)item_name->buf)[0])
      {
        axis_idx = TRIAXIS_Z;
      }

      if(('-' == ((char*)item_str_val->buf)[0] || '+' == ((char*)item_str_val->buf)[0]) &&
          ('x' == ((char*)item_str_val->buf)[1] || 'y' == ((char*)item_str_val->buf)[1] ||
           'z' == ((char*)item_str_val->buf)[1]))
      {
        if('-' == ((char*)item_str_val->buf)[0])
        {
          axis_map[axis_idx].invert = true;
        }

        if('x' == ((char*)item_str_val->buf)[1])
        {
          axis_map[axis_idx].opaxis = TRIAXIS_X;
        }
        else if('y' == ((char*)item_str_val->buf)[1])
        {
          axis_map[axis_idx].opaxis = TRIAXIS_Y;
        }
        else if('z' == ((char*)item_str_val->buf)[1])
        {
          axis_map[axis_idx].opaxis = TRIAXIS_Z;
        }
      }
    }
  }

  return rv;
}

/**Function used to parse N dimensional float array.
 *  
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item 
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type pb_float_arr_arg where parsed 
 *       items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_float_arr(sns_registry_data_item *reg_item,
                             pb_buffer_arg  *item_name,
                             pb_buffer_arg  *item_str_val,
                             void *parsed_buffer)
{
  UNUSED_VAR(item_str_val);
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt)
  {
    pb_float_arr_arg *bias_arr_arg = (pb_float_arr_arg*)parsed_buffer;

    if(*bias_arr_arg->arr_index <= SNS_REGISTRY_MAX_FLT_ARR_LEN)
    {
      bias_arr_arg->arr[*bias_arr_arg->arr_index] = reg_item->flt;
      (*bias_arr_arg->arr_index)++;
    }
    else
    {
      rv = false;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**Function used to parse items belonging to factory calibration
 * 3 * 3 correction matrix registry group 
 *  
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item 
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type matrix3 where parsed items 
 *       are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_corr_matrix_3(sns_registry_data_item *reg_item,
                                 pb_buffer_arg  *item_name,
                                 pb_buffer_arg  *item_str_val,
                                 void *parsed_buffer)
{
  UNUSED_VAR(item_str_val);
  bool rv = true;
  
  if(NULL == reg_item || NULL == item_name || NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt)
  {
    matrix3 *mat = (matrix3 *)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "0_0", item_name->buf_len))
    {
      mat->e00 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "0_1", item_name->buf_len))
    {
      mat->e01 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "0_2", item_name->buf_len))
    {
      mat->e02 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "1_0", item_name->buf_len))
    {
      mat->e10 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "1_1", item_name->buf_len))
    {
      mat->e11 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "1_2", item_name->buf_len))
    {
      mat->e12 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "2_0", item_name->buf_len))
    {
      mat->e20 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "2_1", item_name->buf_len))
    {
      mat->e21 = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "2_2", item_name->buf_len))
    {
      mat->e22 = reg_item->flt;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**Function used to parse items belonging to Motion Detect 
 * Config registry group 
 *  
 * @param[i] reg_item Pointer to sns_registry_data_item
 * @param[i] item_name Pointer to decoded registry item name as
 *       defined in sns_registry_data_item
 * @param[i] item_str_val Pointer to decoded registry item 
 *       string value as defined in sns_registry_data_item
 * @param[o] parsed_buffer of type sns_registry_md_cfg where 
 *       parsed items are stored
 */
SNS_SECTION(".text.sns") bool
sns_registry_parse_md_cfg(sns_registry_data_item *reg_item,
                          pb_buffer_arg  *item_name,
                          pb_buffer_arg  *item_str_val,
                          void *parsed_buffer)
{
  UNUSED_VAR(item_str_val);
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == parsed_buffer)
  {
    rv = false;
  }
  else
  {
    sns_registry_md_cfg *md_cfg = (sns_registry_md_cfg *)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "disable", item_name->buf_len) &&
       reg_item->has_sint)
    {
      md_cfg->disable = (reg_item->sint == 1) ? true : false;
    }
    else if(0 == strncmp((char*)item_name->buf, "thresh", item_name->buf_len) &&
       reg_item->has_flt)
    {
      md_cfg->thresh = reg_item->flt;
    }
    else if(0 == strncmp((char*)item_name->buf, "win", item_name->buf_len) &&
       reg_item->has_flt)
    {
      md_cfg->win = reg_item->flt;
    }
  }

  return rv;
}

