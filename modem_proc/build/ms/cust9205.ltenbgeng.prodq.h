#ifndef CUST9205_LTENBGENG_PRODQ_H
#define CUST9205_LTENBGENG_PRODQ_H
/* ========================================================================
FILE: CUST9205.LTENBGENG.PRODQ

Copyright (c) 2023 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.        
=========================================================================== */

#ifndef TARG9205_LTENBGENG_PRODQ_H
   #include "targ9205.ltenbgeng.prodq.h"
#endif

#define SCL_MODEM_EFS_RAM_SIZE 0x000e0000
#define SCL_SHARED_RAM_BASE 0x87D00000
#define SCL_SHARED_RAM_SIZE 0x100000
#define FEATURE_RMNET_PORT_CONFIG_MSM
#define FEATURE_LTE_REL11
#define FEATURE_LTE_REL12
#define FEATURE_LTE_REL13
#define FEATURE_LTE_REL14
#define FEATURE_CATM_LTE_EXT_GNSS_COEX
#define FEATURE_LTE_IDLE_SVS_CLK
#define CUST_MOB_MODEL_EXT 4108
#define FEATURE_ENHANCED_NW_SELECTION
#define CM_STRING_MAPS_ACTIVE
#define FEATURE_2D_2F_MEAS
#define FEATURE_3GPP_FAST_DORMANCY
#define FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH
#define FEATURE_ACCUM_LOG_TIMESTAMP_AT_FIRST_SAMPLE
#define FEATURE_ALIGNMENT_DIRECTIVE_SYNTAX2
#define FEATURE_AMR_VOCODER
#define FEATURE_APP_SDBTEST
#define FEATURE_APP_W2G
#define FEATURE_AUDIOINIT_TMC_INITS
#define FEATURE_AUTH
#define FEATURE_BARRED_CELL_DURING_CELL_SELECTION
#define FEATURE_BPLMN_SEARCH_320MS
#define FEATURE_BUILD_CDMA_TRIMODE
#define FEATURE_CALL_THROTTLE
#define FEATURE_CCAT
#define FEATURE_CCM_FLOW_CONTROL
#define FEATURE_CELL_FACH_MEAS_OCCASION
#define FEATURE_CLASS_1_IMSI
#define FEATURE_CM
#define FEATURE_CMI
#define FEATURE_CM_CELL_BROADCAST
#define FEATURE_CM_LCS
#define FEATURE_CM_MEASUREMENT
#define FEATURE_CM_PREF_SRV_HOLD_ORIG
#define FEATURE_CM_PWRUP_HOLD_ORIG
#define FEATURE_CM_SEARCH
#define FEATURE_CM_SUPPORTED
#define FEATURE_CM_USE_UMTS_REG_INTERFACE
#define FEATURE_CPHS_EONS
#define FEATURE_CRD_ENHANCEMENTS
#define FEATURE_CSIM
#define FEATURE_CS_FOLLOW_ON_REQUEST
#define FEATURE_DATA
#define FEATURE_DATA_ETSI_SUPSERV
#define FEATURE_DATA_ETSI_SUPSERV_USSD
#define FEATURE_DATA_IS2000_SDB
#define FEATURE_DATA_MM
#define FEATURE_DATA_PS_INTERNAL_AUTH
#define FEATURE_DATA_PS_IN_ALIASES
#define FEATURE_DATA_PS_IPV6
#define FEATURE_DATA_QMI_CAT
#define FEATURE_DATA_RM_NET
#define FEATURE_DBM
#define FEATURE_DC_HSDPA_LOGGING
#define FEATURE_DEEP_SLEEP
#define FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT
#define FEATURE_DETECT_STMR_SLP_CLK_MISMATCH
#define FEATURE_DIAG_DS_SIO_COUPLING
#define FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY
#define FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION
#define FEATURE_DL_BLER_QUAL_CTL
#define FEATURE_DL_CPU_BASED_FC
#define FEATURE_DL_PATH_PROFILING
#define FEATURE_DSAT_FLOW_CONTROL
#define FEATURE_DSAT_INFER_BATT_STATUS
#define FEATURE_DSAT_V80
#define FEATURE_DSM_DIAG_ITEMS
#define FEATURE_DSM_DUP_ITEMS
#define FEATURE_DSM_DYNAMIC_POOL_SELECTION
#define FEATURE_DSM_LARGE_ITEMS
#define FEATURE_DSM_MEM_CHK
#define FEATURE_DSM_QUBE
#define FEATURE_DS_AT_TEST_ONLY
#define FEATURE_DS_CHAP
#define FEATURE_DS_CHOOSE_SO
#define FEATURE_DS_DEFAULT_BITRATE_NV
#define FEATURE_DS_IS2000
#define FEATURE_DS_MTOM
#define FEATURE_DS_NET_MODEL
#define FEATURE_DS_PAP
#define FEATURE_DS_QNC
#define FEATURE_DS_RLP3
#define FEATURE_DS_SOCKETS
#define FEATURE_DS_UI_BAUD
#define FEATURE_DUALMODE_BASELINE
#define FEATURE_DUAL_IP_BEARER
#define FEATURE_DYNAMIC_P_REV
#define FEATURE_DYNAMIC_P_REV_DEFAULT_REL_0
#define FEATURE_EIDLE_SCI
#define FEATURE_EMERGENCY_CATEGORY
#define FEATURE_ENABLE_SLEEP_INT
#define FEATURE_ENABLE_VOICE
#define FEATURE_END_OF_CALL_SPIKE
#define FEATURE_ENHANCED_CELL_RESELECTION
#define FEATURE_EPLMN
#define FEATURE_EQUIVALENT_HPLMN
#define FEATURE_ESTK
#define FEATURE_ETSI_ATTACH
#define FEATURE_ETSI_SMS_CB
#define FEATURE_ETSI_SMS_PS
#define FEATURE_ETSI_SMS_WMS_FIX
#define FEATURE_FACH_IGNORE_S_INTRA_FOR_RESEL
#define FEATURE_FACTORY_TESTMODE
#define FEATURE_FAST_ENC_START_TIME
#define FEATURE_FFE_CGC_WORKAROUND
#define FEATURE_FLEX_FREQ_BIN
#define FEATURE_FREQ_SCAN
#define FEATURE_FTM_EGPRS_BER
#define FEATURE_FTM_GSM_BER
#define FEATURE_G2W_SRCH_LOGGING_ENABLED
#define FEATURE_GERAN_NIKEL_HW_INIT
#define FEATURE_GMM_NV_ITEMS
#define FEATURE_GPRS_CALLCONTROL
#define FEATURE_GPRS_COMP_ENUM_TYPES
#define FEATURE_GPRS_EDA
#define FEATURE_GPRS_LAYER3_TX_CONFIRM
#define FEATURE_GSDI_MULTICLIENT_SUPPORT
#define FEATURE_GSM_AMR
#define FEATURE_GSM_AMR_WB
#define FEATURE_GSM_ARFCN_STRUCT
#define FEATURE_GSM_BAND_PREF
#define FEATURE_GSM_DEEPSLEEP
#define FEATURE_GSM_EGPRS
#define FEATURE_GSM_EGPRS_SAIC
#define FEATURE_GSM_EXT_SPEECH_PREF_LIST
#define FEATURE_GSM_GERAN_FEATURE_PACK_1
#define FEATURE_GSM_GPRS
#define FEATURE_GSM_GPRS_MSC34
#define FEATURE_GSM_GPRS_NV_IF
#define FEATURE_GSM_GPRS_PFC
#define FEATURE_GSM_HALF_RATE
#define FEATURE_GSM_MDSP_AEQ
#define FEATURE_GSM_MDSP_QDSP6
#define FEATURE_GSM_MULTIBAND
#define FEATURE_GSM_NVMEM_ENABLED
#define FEATURE_GSM_PA_TEMP_COMP
#define FEATURE_GSM_PA_TEMP_COMP_PWRLVL
#define FEATURE_GSM_QUADBAND
#define FEATURE_GSM_REMOVE_TRANSITION_PROFILE
#define FEATURE_GSM_RFLM
#define FEATURE_GSM_ROTATOR_SUPPORT
#define FEATURE_GSM_RR_CELL_CALLBACK
#define FEATURE_GSM_R_FACCH
#define FEATURE_GSM_R_SACCH
#define FEATURE_GSM_TCXO_SRM_SUPPORT
#define FEATURE_GSNDCP_USES_COMPTASK
#define FEATURE_GSTK
#define FEATURE_GWSMS
#define FEATURE_GWSMS_ASYNC_SIM_INIT
#define FEATURE_GW_MANUAL_PLMN_IN_PS_CALL
#define FEATURE_GW_MINQOS_PDP_MODIFY
#define FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
#define FEATURE_HHO_ENHANCEMENTS
#define FEATURE_HHO_FAILURE_HANDLING
#define FEATURE_HIT
#define FEATURE_HOME_ONLY
#define FEATURE_HOME_ZONE_SERVICE
#define FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
#define FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
#define FEATURE_HSPA_PLUS_CAT14
#define FEATURE_HSUPA_L1_LOGGING_ENABLED
#define FEATURE_IGNORE_GSM_TRESEL_OOS
#define FEATURE_IGNORE_TRESEL_ON_REACQ_FAIL
#define FEATURE_IGNORE_TRESEL_ON_S_FAIL
#define FEATURE_IMS_FW_API
#define FEATURE_IMS_PROCESS
#define FEATURE_INTERF_SRCH_MEAS
#define FEATURE_INTER_FREQUENCY_BLIND_HANDOVER
#define FEATURE_INTER_FREQUENCY_CELL_FACH_RE_SELECTION
#define FEATURE_INTER_FREQ_EM_CALL_REDIRECTION
#define FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
#define FEATURE_IS683A_PRL
#define FEATURE_IS683C_OTASP
#define FEATURE_IS683C_PRL
#define FEATURE_IS95B
#define FEATURE_IS95B_ALERT_ANS_BYPASS
#define FEATURE_IS95B_MDR
#define FEATURE_IS95B_TRUE_IMSI
#define FEATURE_L1_LOGGING_ENABLED
#define FEATURE_L1_LOG_ON_DEMAND
#define FEATURE_LFS_COOS
#define FEATURE_LIMIT_TFCS_TO_ERAM_SIZE
#define FEATURE_LPM_START_ALL_TASKS
#define FEATURE_MAC_ACTIVATION_TIME
#define FEATURE_MAINTAIN_GTIME_THROUGH_WSLEEP
#define FEATURE_MANUAL_HPLMN_SELECTION
#define FEATURE_MAX_ACCESS_FALLBACK
#define FEATURE_MCC_AND_850_800_BAND_COMBINATION
#define FEATURE_MCC_POWERUP_REG_COUNT
#define FEATURE_MCPM
#define FEATURE_MDSP_DOWNLOAD
#define FEATURE_MDSP_GSM_IMAGE
#define FEATURE_MDSP_UWIRE
#define FEATURE_MEMHEAP_MT
#define FEATURE_MMGSDI
#define FEATURE_MMGSDI_CDMA
#define FEATURE_MMGSDI_GSM
#define FEATURE_MMGSDI_PERSONALIZATION
#define FEATURE_MMGSDI_SESSION_LIB
#define FEATURE_MMGSDI_UMTS
#define FEATURE_MMOC
#define FEATURE_MMOC_SUPPORTS_HYBRID
#define FEATURE_MMOC_TASK
#define FEATURE_MMOC_TMC_FLOW_CNTR
#define FEATURE_MM_SUPERSET
#define FEATURE_MN_LOGGING
#define FEATURE_MODEM_AUDIO
#define FEATURE_MODEM_HEAP
#define FEATURE_MODEM_HW_NIKEL
#define FEATURE_MODEM_RCINIT
#define FEATURE_MODEM_RCINIT_PHASE2
#define FEATURE_MODEM_SOFTWARE
#define FEATURE_MODEM_WDOG_HB
#define FEATURE_MULTIBYTE_SBI
#define FEATURE_MULTIMODE_ARCH
#define FEATURE_MULTIMODE_GSM
#define FEATURE_MULTIMODE_RFAPI
#define FEATURE_MULTI_RF_CHAIN_API
#define FEATURE_MULTI_RX_CHAIN
#define FEATURE_MVS
#define FEATURE_MVS_UNDER_GHDI
#define FEATURE_NAS
#define FEATURE_NAS_KLOCWORK
#define FEATURE_NAS_NVMEM_IMEI
#define FEATURE_NAS_REL7
#define FEATURE_NAS_REMAIN_ON_FORBIDDEN_LAI
#define FEATURE_NAS_VOC_SYNC
#define FEATURE_NETWORK_SHARING
#define FEATURE_NEW_BBRX_SETTINGS
#define FEATURE_NEW_SLEEP_API
#define FEATURE_NIKEL_FW_INTF
#define FEATURE_NOT_USE_MDSP_DMA
#define FEATURE_NV_RUIM
#define FEATURE_OPTIMIZE_SEARCH_FOR_HHO
#define FEATURE_OUT_OF_SERVICE_ENHANCEMENT
#define FEATURE_PBM_TASK
#define FEATURE_PBM_USE_EFS_PB
#define FEATURE_PBM_USIM_SUPPORT
#define FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION
#define FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
#define FEATURE_PIGGYBACK_RESEL
#define FEATURE_PMIC
#define FEATURE_POSIX
#define FEATURE_PREFERRED_MODE_CHANGE
#define FEATURE_PREFERRED_ROAMING
#define FEATURE_PRESERVE_MANUAL_PLMN_LIST
#define FEATURE_PRL_FORCE_MODE
#define FEATURE_PS_FOLLOW_ON_REQUEST
#define FEATURE_QDSP6
#define FEATURE_QMI_SERVICE_SPLIT
#define FEATURE_Q_SINGLE_LINK
#define FEATURE_RAT_PRIORITY_LIST
#define FEATURE_RECONFIG_INDICATOR_CU
#define FEATURE_REL5
#define FEATURE_REL5_25304_130R1
#define FEATURE_REL_99_JUNE_2001
#define FEATURE_REL_ID_NV_SUPPORT
#define FEATURE_RESET_COUNTER
#define FEATURE_RESET_HSIC_CORE
#define FEATURE_REX_APC
#define FEATURE_REX_PROFILE
#define FEATURE_REX_TIMER_EX
#define FEATURE_RFA_CMI_API
#define FEATURE_RFCAL_VERSION
#define FEATURE_RFM_INTERFACE
#define FEATURE_RF_COMMON_LM_RFM_INTERFACE
#define FEATURE_RF_COMP_LIN
#define FEATURE_RF_DUAL_IF_PLL
#define FEATURE_RF_ENABLE_GTOW
#define FEATURE_RF_HAS_L1_INTERFACE
#define FEATURE_RF_HAS_NPA_SCHEDULED_REQUEST_API
#define FEATURE_RF_LOAD_NV_TO_MDSP
#define FEATURE_RF_PLL_DIFF_CLK
#define FEATURE_RF_SVDO_API
#define FEATURE_RF_TASK
#define FEATURE_RF_TEMP_COMP
#define FEATURE_RLP_LOGGING
#define FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX
#define FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
#define FEATURE_RRC_CALLOC
#define FEATURE_RRC_CAPTURE_L1_DEADLOCK
#define FEATURE_RRC_DELAY_CELL_CHANGE_ORDER
#define FEATURE_RRC_DELAY_DCH_FACH
#define FEATURE_RRC_DELAY_ERR_FATAL
#define FEATURE_RRC_ERROR_DETECT
#define FEATURE_RRC_INTEGRITY_SW_ENGINE
#define FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
#define FEATURE_RRC_OPTIMIZED_SIB_HEAP
#define FEATURE_RRC_RELEASE_OPT
#define FEATURE_RRC_SIB_DEBUG
#define FEATURE_RRC_STATIC_AMR
#define FEATURE_RR_ACQ_DB
#define FEATURE_RUIM
#define FEATURE_RUNTIME_RFSETUP_BUF_SIZE
#define FEATURE_RXLM
#define FEATURE_SD20
#define FEATURE_SDCC_8BIT_BUS_SETTING
#define FEATURE_SEC
#define FEATURE_SECONDARY_PDP
#define FEATURE_SEC_IPSEC
#define FEATURE_SEC_SFS
#define FEATURE_SEC_SSL
#define FEATURE_SET_BAND_IN_GL1
#define FEATURE_SIB11BIS_SUPPORT
#define FEATURE_SIB18
#define FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
#define FEATURE_SLEEP
#define FEATURE_SLEEP_GSM
#define FEATURE_SLEEP_TIMETAG
#define FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
#define FEATURE_SMC_SRB2_SUSPENSION
#define FEATURE_SMD_LITE
#define FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
#define FEATURE_SNM_ENHANCEMENTS
#define FEATURE_SRB5_SUPPORT
#define FEATURE_SSPR_800
#define FEATURE_SSPR_ENHANCEMENTS
#define FEATURE_STAY_CAMPED_ON_ACCESS_CLASS_BARRED_CELL
#define FEATURE_SUPPORT_FOR_PARTIAL_ROAMING
#define FEATURE_SUSPEND_WITH_ACT_TIME
#define FEATURE_SYNC_DET_TIMER
#define FEATURE_SYSTEM_ARBITRATION
#define FEATURE_TCXOMGR_DYN_RPUSH
#define FEATURE_TCXOMGR_NEGATE_FREQ_ERROR
#define FEATURE_TC_CONSTANT_DELAY
#define FEATURE_TC_CONSTANT_DELAY_ENH
#define FEATURE_TFCC_ENHNC
#define FEATURE_TMC_TCXOMGR
#define FEATURE_TMC_TRM
#define FEATURE_TM_LB
#define FEATURE_TRANS_MANAGER
#define FEATURE_TTY
#define FEATURE_TUNNELED_SMS_OVER_CSFB
#define FEATURE_TXLM
#define FEATURE_TX_POWER_BACK_OFF
#define FEATURE_T_BARRED_SUPPORT
#define FEATURE_UART_TCXO_CLK_FREQ
#define FEATURE_UE_INITIATED_DORMANCY
#define FEATURE_UICC_RAT_INDICATOR_SUPPORT
#define FEATURE_UIM
#define FEATURE_UIM_CAVE_AN_AUTH
#define FEATURE_UIM_DISABLE_GSDI_INTERFACE
#define FEATURE_UIM_DRIVER
#define FEATURE_UIM_RUIM
#define FEATURE_UIM_RUN_TIME_ENABLE
#define FEATURE_UIM_SUPPORT_LBS
#define FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
#define FEATURE_UI_STRING_MAPS_ACTIVE
#define FEATURE_UL_CPU_BASED_FC
#define FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
#define FEATURE_UMTS_CIPHERING_ENABLE
#define FEATURE_UMTS_CONC_SRVC
#define FEATURE_UMTS_PDCP
#define FEATURE_UNIFIED_SLEEP_CTLR_P1
#define FEATURE_UNIFIED_SLEEP_CTLR_P2
#define FEATURE_UNIFIED_SLEEP_TASK
#define FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
#define FEATURE_USIM_1000_PBM
#define FEATURE_UW_FMC
#define FEATURE_VAMOS
#define FEATURE_VBATT_DUMMY_BATTERY
#define FEATURE_VOC_4GV_NW
#define FEATURE_VOC_4GV_WB
#define FEATURE_VOC_AMR_WB
#define FEATURE_VOIP
#define FEATURE_WDOG_DYNAMIC
#define FEATURE_WFW_SW_INTERFACE
#define FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
#define FEATURE_WTOG_NACC
#define FEATURE_WTOG_REDIRECTION_AFTER_CONN_RELEASE
#define FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
#define FEATURE_XO
#define FEATURE_XO_FACTORY_CAL
#define FEATURE_XO_FCR
#define FEATURE_XO_TASK
#define GERAN_NIKEL_FW_INTERFACE
#define L1_UL_USE_DSM
#define RF_FEATURE_LTOG
#define RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
#define RF_HAS_CDMA_DIME_SUPPORT
#define RF_HAS_GSM_CCS_FW_INTF
#define RF_HAS_RFA_GSM_TASK
#define RF_MULTIMODE
#define TMC_SLEEP
#define T_QDSP6
#define UNICORE_MODEM
#define MOB_P_REV 9
#define SCL_APPS_BOOT_BASE 0x00000000
#define SCL_APPS_BOOT_SIZE 0x0100000
#define IMAGE_KEY_MBA_IMG_DEST_ADDR 0xFCC08000
#define FEATURE_QFUSE_PROGRAMMING
#define FEATURE_Q_NO_SELF_QPTR
#define FEATURE_DOG_BARK
#define DIAG_FEATURE_EFS2
#define DIAG_IMAGE_MODEM_PROC
#define DIAG_TOOLSDIAG_INIT
#define DIAG_TOOLSDIAG_ESN
#define DIAG_RSP_SEND
#define DIAG_SIO_SUPPORT 
#define FEATURE_DIAG_NV 
#define DIAG_CHECK_SECURITY
#define DIAG_C_STRING 
#define DIAG_DIAGTBL_INIT
#define DIAG_DOG_SUPPORT
#define DIAG_DEFAULT_BITRATE_NV
#define FEATURE_DIAG_HW_ADDR_CHECK
#define FEATURE_DIAG_MANUAL_DISPATCH_TABLE
#define DIAG_MAX_RX_PKT_SIZ 4096
#define DIAG_RSP_HEAP_SIZE (1024 * 8)
#define DIAGBUF_SIZE (1024*32*2)
#define DIAG_F3_TRACE_BUFFER_SIZE 32768
#define DIAG_EVENT_HEAP_SIZE (1024 * 8)
#define ULOG_MAXIMUM_LOG_SIZE (1024 * 4)
#define MAX_MSG_V4_SSID_TABLE_ENTRY 512
#define MAX_MSG_V4_SSID_AAM_TABLE_ENTRY 512
#define FEATURE_ERR_EXTENDED_STORE
#define FEATURE_SAVE_DEBUG_TRACE
#define FEATURE_SAVE_TRACE_EXTENDED
#define FEATURE_OPTIMIZE_TRACE_RECORDS
#define FEATURE_SAVE_TRACE_ON_BY_DEFAULT
#define FEATURE_ERR_HAS_F3_TRACE
#define ERR_MODEM_PROC
#define FEATURE_QURT
#define FEATURE_DAL
#define FEATURE_QUBE
#define DEVCFG_DATA_SEG_SIZE 0x19000
#define DEVCFG_PLATFORM_DATA_SEG_SIZE 0x4000
#define FEATURE_MULTIPROCESSOR
#define FEATURE_SMEM
#define FEATURE_SMSM
#define FEATURE_SMSM_DEBUG_LOGGING
#define FEATURE_SMEM_LOG
#define FEATURE_SMD
#define FEATURE_SMDL_LOOPBACK_TEST
#define FEATURE_IPC_ROUTER
#define FEATURE_IPC_ROUTER_XAL_SMDL
#define FEATURE_QSAP_SERVICE
#define IPC_ROUTER_XAL_SMDL_PARAM_1 {"IPCRTR", SMD_APPS_MODEM, SMD_STANDARD_FIFO, SMDL_OPEN_FLAGS_MODE_PACKET, TRUE}
#define FEATURE_DSM_WM_CB 
#define FEATURE_CMI_WM_CB 
#define FEATURE_DSM_LOCKLESS
#define FEATURE_MLOG_PROFILING
#define FEATURE_USE_TIME_VU
#define MAX_TIMER_BUFFER_NUMBER 100
#define MIN_TIMER_BUFFER_NUMBER -1
#define TIMER_BIN_NUM 64
#define TIMER_BIN_RANGE 9600000
#define MAX_TIMER_EXPIRED_SLAVE1 30
#define MAX_TIMER_EXPIRED_SLAVE2 30
#define MAX_TIMER_EXPIRED_SLAVE3 30
#define FEATURE_RUNTIME_DEVMAP
#define FEATURE_RDEVMAP_DS_DEFAULT_TO_USB
#define RDM_USES_FSHS_USB
#define RDM_UART1_INIT_SRVC RDM_DIAG_SRVC
#define RDM_USB_MDM_INIT_SRVC RDM_DATA_SRVC
#define FEATURE_USB_CDC_ACM
#define FEATURE_NO_SIO_USB_DEV_INIT
#define FEATURE_RDM_ALWAYS_NON_PERSISTENT
#define FEATURE_DS
#define FEATURE_DATA_PS
#define FEATURE_GSM
#define FEATURE_POLAR
#define FEATURE_RFGSM_NONSIG_GERAN_API 
#define FEATURE_NB_JAMMER_DETECTION
#define FEATURE_MODEM_COEXISTENCE_SW
#define FEATURE_TCXOMGR_CLIENT_RGS_API
#define FEATURE_MCS_TCXO_REFACTORED
#define FEATURE_TCXOMGR_MSIM_RPUSH_API
#define FEATURE_TRM_V6
#define FEATURE_MCS_WWCOEX_UNIFIED_PRIORITY
#define FEATURE_RF
#define FEATURE_BUILD_CDMA_CELL_PCS
#define FEATURE_INTELLICEIVER
#define FEATURE_LIN_INTERPOLATION_FREQ_COMP
#define FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define FEATURE_PA_ACCESS_PROBE_CONFIG
#define FEATURE_MSM_LIBRARY
#define FEATURE_RF_HAS_NPA_SLEEP_ENABLED
#define WTR2965_HAS_NO_CA_SUPPORT
#define FEATURE_FTM_HWTC
#define FEATURE_RF_HAS_MEMORY_OPT
#define FEATURE_RF_TX_PWR_LIM
#define FEATURE_RF_ENABLE_CM
#define FEATURE_RF_NEW_ARCH_FOR_UI_DEBUG_SCREEN
#define FEATURE_DEDICATED_PRIORITY
#define FEATURE_MCPM_NPA_SCHEDULED_REQ_SUPPORT
#define FEATURE_MSM_SURF_CONFIG4
#define FEATURE_NAS_MODE_CHANGE
#define FEATURE_NAS_REL10
#define FEATURE_NV_MEID_ENABLE 
#define FEATURE_SURF_CLK_SRC_PLL
#define FEATURE_UI_PBM
#define TMC_RCINIT_REXTASK_TLM
#define TMC_RCINIT_REXTASK_XTM
#define T_8974V1
#define FEATURE_SLPC_TRIPLE_SIM
#define MEMDEBUG_DB_SIZE_SHFT 15
#define FEATURE_MPPS_CPP_AGGREGATOR
#define FEATURE_MPSS_VUNICORE
#define FEATURE_FPLMN_MAX_LENGTH
#define FEATURE_IP_CALL
#define FEATURE_PBR_MARKOV
#define FEATURE_MPSS_DI_2_0
#define FEATURE_FATAL_SECTION
#define FEATURE_RTRE_DEFAULT_IS_NV
#define FEATURE_JOLOKIA_MODEM
#define DIAG_MSG_DISABLE_LVL_MASK 3
#define FEATURE_DATA_PS_464XLAT
#define FEATURE_DATAMODEM_DNS
#define FEATURE_EFS2
#define FS_MAX_PAGES_PER_BLOCK 64
#define EFS_MAX_FILE_DESCRIPTORS 10
#define FS_PATH_MAX 256
#define FS_NAME_MAX 256
#define EFS_PAGE_SIZE 2048
#define FLASH_ENABLE_NAND_SPI
#define FS_API_DEBUG_DISABLE
#define DSMI_DS_SMALL_ITEM_CNT 800
#define DSMI_DS_LARGE_ITEM_CNT 200
#define DSMI_DUP_ITEM_CNT 2500 
#define FEATURE_CM_LTE
#define FEATURE_DATA_LTE
#define FEATURE_LTE_3GPP_REL8_MAR09
#define FEATURE_LTE_REL10
#define FEATURE_LTE_REL9
#define FEATURE_LTE_ZUC_INTEGRITY
#define FEATURE_DOMAIN_SELECTION
#define FEATURE_LTE
#define FEATURE_A2
#define APPMGR_NUM_APPS 1
#define FEATURE_MODEM_STANDALONE
#define FEATURE_MODEM_TARGET_BUILD
#define FEATURE_LTE_FW_CONTROLS_TX_GRFC
#define FEATURE_LTE_FULL_STACK
#define FEATURE_LTE_TO_GSM_CGI
#define FEATURE_LTE_TO_GSM_CCO
#define LTE_3GPP_DEFAULT_RELEASE_VERSION LTE_3GPP_REL14
#define FEATURE_CM_NB1
#define FEATURE_DATA_NB1
#define FEATURE_NB1_3GPP_REL8_MAR09
#define FEATURE_NB1_REL10
#define FEATURE_NB1_REL9
#define FEATURE_NB1_ZUC_INTEGRITY
#define FEATURE_SMS_PWS_OVER_NB1
#define FEATURE_NB1
#define FEATURE_A2
#define APPMGR_NUM_APPS 1
#define FEATURE_MODEM_STANDALONE
#define FEATURE_MODEM_TARGET_BUILD
#define FEATURE_NB1_FW_CONTROLS_TX_GRFC
#define FEATURE_NB1_FULL_STACK
#define NB1_3GPP_DEFAULT_RELEASE_VERSION NB1_3GPP_REL10_SEP12
#define FEATURE_NB1_PLT
#define FEATURE_AUDIO_TX_MEM_OPT
#define FEATURE_STATIC_AMSS_HEAP
#define FEATURE_STATIC_MODEM_HEAP
#define FEATURE_SPLIT_MODEM_HEAP
#define MODEM_MEM_HEAP_SIZE 0X90000
#define MODEM_INTERNAL_HEAP_SIZE 0x60000
#define MODEM_INTERNAL_HEAP_ALLOC_RANGE 2048
#define FEATURE_CLADE_HEAP
#define CLADE_HEAP_SIZE 0xC0000
#define AMSS_MEMHEAP_HEAP_SIZE 0x1C0000
#define FEATURE_EXTPL_HEAP
#define EXTPL_HEAP_SIZE 0xC0000
#define FEATURE_IMS_SMS
#define FEATURE_IMS_VOLTE
#define FEATURE_USSD_PS_ENABLED
#define FEATURE_EXTERNAL_PAGING
#define FEATURE_EXTPL_TEST
#define FEATURE_DYNAMIC_LOADING_LTE
#define FEATURE_DYNAMIC_LOADING_NB1
#define FEATURE_DYNAMIC_LOADING_GERAN
#define FEATURE_DATAMODEM_DSS_PING
#define FEATURE_DATAMODEM_SEC_SSL
#define FEATURE_FSK_TX
#define FEATURE_ENABLE_GSM_VOCODER
#define FEATURE_RF_SUPPORT_BAND_87_88


#include "custglobal.h"
#include "custmodem.h"

#ifdef FEATURE_DATA_DSD
   #undef FEATURE_DATA_DSD
#endif
#ifdef FEATURE_DS_MOBILE_IP_DMU
   #undef FEATURE_DS_MOBILE_IP_DMU
#endif
#ifdef FEATURE_CBSPTEST
   #undef FEATURE_CBSPTEST
#endif
#ifdef FEATURE_CGPS_USES_CDMA
   #undef FEATURE_CGPS_USES_CDMA
#endif
#ifdef FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS
   #undef FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS
#endif

#endif /* CUST9205_LTENBGENG_PRODQ_H */
