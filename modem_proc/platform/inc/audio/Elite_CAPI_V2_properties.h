#ifndef ELITE_CAPI_V2_PROPERTIES_H
#define ELITE_CAPI_V2_PROPERTIES_H

/* ========================================================================*/
/**
 @file Elite_CAPI_V2_properties.h
 Common Audio Processing Interface v2 header file

 This file defines the data structures and ids for getting and setting
 properties in the Common Audio Processing Interface.
 */

/*======================================================================
 Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

#include "Elite_CAPI_V2_types.h"
#include "Elite_CAPI_V2_events.h"

/* Property IDs
 *
 * Properties are used to set and get information to and from the module. Properties are identified by ids
 * and have corresponding payloads. Their usage is similar to parameters, but parameters are module specific -
 * they are defined by the implementor of the module. Parameters are used to control aspects which are
 * specific to the underlying algorithm. Properties are generic, and are defined in the CAPI_V2 interface.
 */
enum capi_v2_property_id_t
{
   //----------- Properties that can be queried statically using get_static_properties ---------------------------------
   CAPI_V2_INIT_MEMORY_REQUIREMENT, /**< The amount of memory in bytes to be passed into the capi_v2_init function.
    Payload structure: capi_v2_init_memory_requirement_t. */
   CAPI_V2_STACK_SIZE, /**< The amount of stack size in bytes needed by this module
    Payload Structure: capi_v2_stack_size_t */
   CAPI_V2_MAX_METADATA_SIZE, /**< The maximum size of metadata generated by this module after each call
    to process(). If this value is zero, the module does not generate any
    metadata. It includes size of different structures used to pack
    metadata (See property CAPI_V2_METADATA).
    Payload Structure: capi_v2_max_metadata_size_t */
   CAPI_V2_IS_INPLACE, /**< This indicates whether the module can perform in-place computation. If this value
    is true, the caller may provide the same pointers for input and output (but this
    is not guaranteed). This requires that the input and output data format be the
    same and the requires_data_buffering property be false.
    Payload Structure: capi_v2_is_inplace_t */
   CAPI_V2_REQUIRES_DATA_BUFFERING, /**< If this value is false, the module must behave as follows:
    1. The number of output samples should always be the same as the number
    of input samples on all output ports. The caller must ensure that the
    number of input samples is the same on all input ports.
    2. All the input must be consumed. The caller must ensure that the output
    buffers have enough space.
    3. The module should be able to handle any number of samples.

    If this value is true, the module must behave as follows:
    1. The module must define a threshold in terms of number of bytes for each
    input port and each output port.
    2. The module must consume data on its inputs and fill data on its outputs
    till the amount of remaining data on each buffer of at least one input
    port is less than its threshold or the amount of free space on each buffer
    of at least one output port is less than its threshold.

    Note: Setting this value to true adds significant overhead, so it should
    only be used if:
    1. The module performs encoding/decoding of data.
    2. The module performs rate conversion between the input and output.

    Payload Structure: capi_v2_requires_data_buffering_t
    */
   CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS, /**< The number of framework extensions needed by this module.
    Payload Structure: capi_v2_num_needed_framework_extensions_t */
   CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, /**< The list of framework extensions that are needed by this module.
    Payload Structure: An array of capi_v2_framework_extension_id_t structures. Each value is the id of a
    framework extension. The number of elements of the array is the number returned in the query for the
    property CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS. */

   CAPI_V2_MAX_STATIC_PROPERTIES = 0x10000, /**< Dummy value to indicate the range */
   //------------ End of properties that can be queried statically -------------------------

   //-------------- Properties that can be set at init and at any time after init ----------------------------
   CAPI_V2_EVENT_CALLBACK_INFO, /**< The function pointer and state needed for raising events to the framework.
    Payload Structure: capi_v2_event_callback_info_t */
   CAPI_V2_PORT_NUM_INFO, /**< Sets the number of input and output ports for the module.
    Payload Structure: capi_v2_port_num_info_t */
   CAPI_V2_HEAP_ID, /**< Provides the heap IDs for allocating memory.
    Payload Structure: capi_v2_heap_id_t */
   CAPI_V2_INPUT_MEDIA_FORMAT, /**< Sets the media format for an input port.
    The port id must be set in the payload by the caller.
    Payload Structure: capi_v2_set_get_media_format_t */
   CAPI_V2_MAX_INIT_PROPERTIES = 0x20000, /**< Dummy value to indicate the range */
   //-------------- End of properties that can be set at init and at any time after init ----------------------

   //-------------- Properties that can only be set after init -------------
   CAPI_V2_ALGORITHMIC_RESET, /**< When this property is set, the module must reset any internal buffers and
    algorithm state to zero. Any settings should not be reset. Any memory allocated by the module need not be freed.
    Payload Structure: empty buffer */
   CAPI_V2_EXTERNAL_SERVICE_ID, /**< This is used to provide the id of an external service that the module
    can communicate with. The communication is achieved by raising the
    data_to_other_service event. An identifier is passed to the module with
    the service ID. The value of the indentifier is set by the client
    to indicate to the module what his service id is for.
    Payload Structure: capi_v2_external_service_id_t */
   CAPI_V2_MAX_SET_PROPERTIES = 0x30000, /**< Dummy value to indicate the range */
   //-------------- End of properties that can be set any time -------------

   //-------------- Properties that can only be queried only using get properties ---------------
   CAPI_V2_METADATA,
   /**< The module must fill in any metadata that it generated during a process() call when
    * the caller queries this parameter. The query is typically done after the module raises the event CAPI_V2_EVENT_METADATA_AVAILABLE.
    * Metadata is put in the payload of capi_v2_metadata_t.
    * The module should pack the payload of metadata as groups of asm_stream_param_data_t (defined in adsp_asm_stream_commands.h).
    * The buffer is structured as follows:
    * 1st asm_stream_param_data_v2_t
    * 1st metadata
    * 2nd asm_stream_param_data_v2_t
    * 2nd metadata
    * and so on.
    *
    * Payload Structure: capi_v2_metadata_t
    * */
   CAPI_V2_PORT_DATA_THRESHOLD,
   /**< The threshold in bytes of an input or output port. This property is only used for modules that
    * require data buffering. Refer to the comments for the CAPI_V2_REQUIRES_DATA_BUFFERING property for usage.
    * Payload Structure: capi_v2_port_data_threshold_t
    */
   CAPI_V2_OUTPUT_MEDIA_FORMAT_SIZE,
   /**< The size of the media format payload for a particular output port. This excludes the size of
    * capi_v2_data_format_header_t.
    * Payload Structure: capi_v2_output_media_format_size_t
    */
   CAPI_V2_MAX_GET_PROPERTIES = 0x40000,

   //-------------- Properties that can be set or queried using set/get properties only ------------
   CAPI_V2_OUTPUT_MEDIA_FORMAT,
   /**< Can be used to query the media format for a particular output port.
    * This property can also be used to set the output media format for modules at support control
    * of the output media format. If a module only supports controlling some aspects like say the
    * sample rate only, all other fields can be set to CAPI_V2_DATA_FORMAT_INVALID_VAL.
    * The port id must be set in the payload by the caller.
    *
    * Payload Structure: capi_v2_set_get_media_format_t
    */
   CAPI_V2_MAX_SET_GET_PROPERTIES = 0x50000,

   CAPI_V2_MAX_PROPERTY = 0x7FFFFFFF
};

/**
 Structure for a property that can be sent to a module. Properties are used for
 generic set and get which are independent of the underlying module.
 */

typedef enum capi_v2_property_id_t capi_v2_property_id_t;

typedef struct capi_v2_prop_t capi_v2_prop_t;
struct capi_v2_prop_t
{
   capi_v2_property_id_t id;
   /**< Unique identifier of the property that is being sent. */

   capi_v2_buf_t payload;
   /**< The buffer must contain the payload corresponding to the
    * property value for the set_property call, and it must be
    * sufficiently large to contain the payload for a get_property
    * call. */

   capi_v2_port_info_t port_info;
   /**< information about the port for which this property is applicable. In case the property
    * is not port specific or is applicable to all the ports then port info must have is_valid flag set as false.*/
};

/**
 Structure for a list of CAPI_V2 properties. It can be used to send down
 or query a list of properties to the module.
 */
typedef struct capi_v2_proplist_t capi_v2_proplist_t;
struct capi_v2_proplist_t
{
   uint32_t props_num; /**< Number of elements in the array. */
   capi_v2_prop_t* prop_ptr; /**< Array of CAPI_V2 property elements. */
};

// Payloads for the properties
typedef struct capi_v2_init_memory_requirement_t capi_v2_init_memory_requirement_t;
struct capi_v2_init_memory_requirement_t
{
   uint32_t size_in_bytes;
};

typedef struct capi_v2_stack_size_t capi_v2_stack_size_t;
struct capi_v2_stack_size_t
{
   uint32_t size_in_bytes;
};

typedef struct capi_v2_max_metadata_size_t capi_v2_max_metadata_size_t;
struct capi_v2_max_metadata_size_t
{
   uint32_t output_port_index;
   uint32_t size_in_bytes;
};

typedef struct capi_v2_is_inplace_t capi_v2_is_inplace_t;
struct capi_v2_is_inplace_t
{
   bool_t is_inplace;
};

typedef struct capi_v2_requires_data_buffering_t capi_v2_requires_data_buffering_t;
struct capi_v2_requires_data_buffering_t
{
   bool_t requires_data_buffering;
};

typedef struct capi_v2_event_callback_info_t capi_v2_event_callback_info_t;
struct capi_v2_event_callback_info_t
{
   capi_v2_event_cb_f event_cb; /* Callback function to be used to raise an event. */
   void *event_context; /* An opaque pointer value used as the context for this function. */
};

typedef struct capi_v2_port_num_info_t capi_v2_port_num_info_t;
struct capi_v2_port_num_info_t
{
   uint32_t num_input_ports;
   uint32_t num_output_ports;
};

typedef struct capi_v2_heap_id_t capi_v2_heap_id_t;
struct capi_v2_heap_id_t
{
   uint32_t heap_id;
};

typedef struct capi_v2_external_service_id_t capi_v2_external_service_id_t;
struct capi_v2_external_service_id_t
{
   uint32_t identifier;
   uint32_t service_id;
};

typedef struct capi_v2_metadata_t capi_v2_metadata_t;
struct capi_v2_metadata_t
{
   capi_v2_buf_t payload;
   // See description for CAPI_V2_METADATA property for details.
};

typedef struct capi_v2_port_data_threshold_t capi_v2_port_data_threshold_t;
struct capi_v2_port_data_threshold_t
{
   uint32_t threshold_in_bytes;
};

typedef struct capi_v2_output_media_format_size_t capi_v2_output_media_format_size_t;
struct capi_v2_output_media_format_size_t
{
   uint32_t size_in_bytes;
};

typedef struct capi_v2_num_needed_framework_extensions_t capi_v2_num_needed_framework_extensions_t;
struct capi_v2_num_needed_framework_extensions_t
{
   uint32_t num_extensions;
};

typedef struct capi_v2_framework_extension_id_t capi_v2_framework_extension_id_t;
struct capi_v2_framework_extension_id_t
{
   uint32_t id;
};


#endif /* #ifndef ELITE_CAPI_V2_PROPERTIES_H */
