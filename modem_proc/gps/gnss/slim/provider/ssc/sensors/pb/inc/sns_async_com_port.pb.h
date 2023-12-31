/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.7 at Fri Jul 14 02:01:36 2017. */

#ifndef PB_SNS_ASYNC_COM_PORT_PB_H_INCLUDED
#define PB_SNS_ASYNC_COM_PORT_PB_H_INCLUDED
#include <pb.h>

#include "sns_std.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _sns_async_com_port_operation_type {
    SNS_ASYNC_COM_PORT_OPERATION_TYPE_READ = 0,
    SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE = 1
} sns_async_com_port_operation_type;
#define _sns_async_com_port_operation_type_MIN SNS_ASYNC_COM_PORT_OPERATION_TYPE_READ
#define _sns_async_com_port_operation_type_MAX SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE
#define _sns_async_com_port_operation_type_ARRAYSIZE ((sns_async_com_port_operation_type)(SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE+1))
#define sns_async_com_port_operation_type_SNS_ASYNC_COM_PORT_OPERATION_TYPE_READ SNS_ASYNC_COM_PORT_OPERATION_TYPE_READ
#define sns_async_com_port_operation_type_SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE

typedef enum _sns_async_com_port_bus_type {
    SNS_ASYNC_COM_PORT_BUS_TYPE_I2C = 0,
    SNS_ASYNC_COM_PORT_BUS_TYPE_SPI = 1
} sns_async_com_port_bus_type;
#define _sns_async_com_port_bus_type_MIN SNS_ASYNC_COM_PORT_BUS_TYPE_I2C
#define _sns_async_com_port_bus_type_MAX SNS_ASYNC_COM_PORT_BUS_TYPE_SPI
#define _sns_async_com_port_bus_type_ARRAYSIZE ((sns_async_com_port_bus_type)(SNS_ASYNC_COM_PORT_BUS_TYPE_SPI+1))
#define sns_async_com_port_bus_type_SNS_ASYNC_COM_PORT_BUS_TYPE_I2C SNS_ASYNC_COM_PORT_BUS_TYPE_I2C
#define sns_async_com_port_bus_type_SNS_ASYNC_COM_PORT_BUS_TYPE_SPI SNS_ASYNC_COM_PORT_BUS_TYPE_SPI

typedef enum _sns_async_com_port_reg_addr_type {
    SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT = 0,
    SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_16_BIT = 1,
    SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_32_BIT = 2
} sns_async_com_port_reg_addr_type;
#define _sns_async_com_port_reg_addr_type_MIN SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT
#define _sns_async_com_port_reg_addr_type_MAX SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_32_BIT
#define _sns_async_com_port_reg_addr_type_ARRAYSIZE ((sns_async_com_port_reg_addr_type)(SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_32_BIT+1))
#define sns_async_com_port_reg_addr_type_SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT
#define sns_async_com_port_reg_addr_type_SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_16_BIT SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_16_BIT
#define sns_async_com_port_reg_addr_type_SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_32_BIT SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_32_BIT

typedef enum _sns_async_com_port_msgid {
    SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG = 512,
    SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW = 1024,
    SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR = 1025,
    SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_REG = 1026
} sns_async_com_port_msgid;
#define _sns_async_com_port_msgid_MIN SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG
#define _sns_async_com_port_msgid_MAX SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_REG
#define _sns_async_com_port_msgid_ARRAYSIZE ((sns_async_com_port_msgid)(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_REG+1))
#define sns_async_com_port_msgid_SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG
#define sns_async_com_port_msgid_SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW
#define sns_async_com_port_msgid_SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR
#define sns_async_com_port_msgid_SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_REG SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_REG

/* Struct definitions */
typedef struct _sns_async_com_port_config {
    sns_async_com_port_bus_type bus_type;
    uint32_t slave_control;
    sns_async_com_port_reg_addr_type reg_addr_type;
    uint32_t min_bus_speed_kHz;
    uint32_t max_bus_speed_kHz;
    uint32_t bus_instance;
/* @@protoc_insertion_point(struct:sns_async_com_port_config) */
} sns_async_com_port_config;

typedef struct _sns_async_com_port_error {
    uint32_t reg_addr;
    uint32_t num_bytes;
    sns_std_error error_code;
/* @@protoc_insertion_point(struct:sns_async_com_port_error) */
} sns_async_com_port_error;

typedef struct _sns_async_com_port_vector {
    sns_async_com_port_operation_type operation;
    uint32_t reg_addr;
    bool has_num_bytes;
    uint32_t num_bytes;
    pb_callback_t buffer;
/* @@protoc_insertion_point(struct:sns_async_com_port_vector) */
} sns_async_com_port_vector;

typedef struct _sns_async_com_port_vector_rw {
    uint32_t num_vectors;
    pb_callback_t vectors;
/* @@protoc_insertion_point(struct:sns_async_com_port_vector_rw) */
} sns_async_com_port_vector_rw;

/* Default values for struct fields */

/* Initializer values for message structs */
#define sns_async_com_port_config_init_default   {(sns_async_com_port_bus_type)0, 0, (sns_async_com_port_reg_addr_type)0, 0, 0, 0}
#define sns_async_com_port_vector_init_default   {(sns_async_com_port_operation_type)0, 0, false, 0, {{NULL}, NULL}}
#define sns_async_com_port_vector_rw_init_default {0, {{NULL}, NULL}}
#define sns_async_com_port_error_init_default    {0, 0, (sns_std_error)0}
#define sns_async_com_port_config_init_zero      {(sns_async_com_port_bus_type)0, 0, (sns_async_com_port_reg_addr_type)0, 0, 0, 0}
#define sns_async_com_port_vector_init_zero      {(sns_async_com_port_operation_type)0, 0, false, 0, {{NULL}, NULL}}
#define sns_async_com_port_vector_rw_init_zero   {0, {{NULL}, NULL}}
#define sns_async_com_port_error_init_zero       {0, 0, (sns_std_error)0}

/* Field tags (for use in manual encoding/decoding) */
#define sns_async_com_port_config_bus_type_tag   1
#define sns_async_com_port_config_slave_control_tag 2
#define sns_async_com_port_config_reg_addr_type_tag 3
#define sns_async_com_port_config_min_bus_speed_kHz_tag 4
#define sns_async_com_port_config_max_bus_speed_kHz_tag 5
#define sns_async_com_port_config_bus_instance_tag 6
#define sns_async_com_port_error_reg_addr_tag    1
#define sns_async_com_port_error_num_bytes_tag   2
#define sns_async_com_port_error_error_code_tag  3
#define sns_async_com_port_vector_operation_tag  1
#define sns_async_com_port_vector_reg_addr_tag   2
#define sns_async_com_port_vector_num_bytes_tag  3
#define sns_async_com_port_vector_buffer_tag     4
#define sns_async_com_port_vector_rw_num_vectors_tag 1
#define sns_async_com_port_vector_rw_vectors_tag 2

/* Struct field encoding specification for nanopb */
extern const pb_field_t sns_async_com_port_config_fields[7];
extern const pb_field_t sns_async_com_port_vector_fields[5];
extern const pb_field_t sns_async_com_port_vector_rw_fields[3];
extern const pb_field_t sns_async_com_port_error_fields[4];

/* Maximum encoded size of messages (where known) */
#define sns_async_com_port_config_size           24
/* sns_async_com_port_vector_size depends on runtime parameters */
/* sns_async_com_port_vector_rw_size depends on runtime parameters */
#define sns_async_com_port_error_size            21

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define SNS_ASYNC_COM_PORT_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
