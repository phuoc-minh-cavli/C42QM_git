#ifndef QDSS_TFUNNEL_H
#define QDSS_TFUNNEL_H

/*=============================================================================

  @file  qdss_tfunnel.h
  @brief QUALCOMM Debug Subsystem (QDSS) API for enabling/disabling speicific
         trace funnel ports

================================================================================
              Copyright (c) 2013, 2019 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/


#define TFUNNEL_RPM_PORT             "port_rpm"
#define TFUNNEL_MPSS_PORT            "port_mpss"
#define TFUNNEL_ADSP_PORT            "port_adsp"
#define TFUNNEL_SYSTEM_NOC_PORT      "port_system_noc"
#define TFUNNEL_APPS_ETM_PORT        "port_apps_etm"
#define TFUNNEL_MMSS_NOC_PORT        "port_mmss_noc"
#define TFUNNEL_PERIPHERAL_NOC_PORT  "port_peripheral_noc"
#define TFUNNEL_DEBUG_UI_PORT        "port_debug_ui"
#define TFUNNEL_RPM_ITM_PORT         "port_rpm_itm"
#define TFUNNEL_MMSS_PORT            "port_mmss"
#define TFUNNEL_PRONTO_PORT          "port_pronto"
#define TFUNNEL_BIMC_PORT            "port_bimc"
#define TFUNNEL_MODEM_PORT           "port_modem"
#define TFUNNEL_OCMEM_NOC_PORT       "port_ocmem_noc"
#define TFUNNEL_STM_PORT             "port_stm"
#define TFUNNEL_TPDA_QDSS_PORT       "port_tpda_qdss"
#define TFUNNEL_TPDA_DCC_PORT        "port_tpda_dcc"

/**
  @brief Enable a specific trace funnel port.

  This function enables trace data flow through a named funnel port.
  When a funnel port is enabled the required merge funnel port is also enabled.
  However trace sources and trace sinks have to be enabled separately
  to complete the trace data path

  @param port_name [in]: Name of the funnel port to be enabled.
                         Accepted names are listed above

  @return 0 on success. A non-zero error code on failure
*/
int qdss_tfunnel_enable_port(const char *port_name); 


/**
  @brief Disable a specific trace funnel port.

  This function disables trace data flow through a named funnel port.

  @param port_name [in]: Name of the funnel port to be disbled.
                         Accepted names are listed above

  @return 0 on success. A non-zero error code on failure
*/
int qdss_tfunnel_disable_port(const char *port_name); 

/**
  @brief Disable a specific trace funnel port.

  This function disables all trace funnel ports and merge funnel ports.
  This is useful if trace data from all other sources needs to be shut off,
  before enabling trace data from just the source you are interested in

  @return 0 on success. A non-zero error code on failure
*/

int qdss_tfunnel_disable_all_ports(void); 


#endif //QDSS_TFUNNEL_H
