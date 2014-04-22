#include "../config/devcfg_lpasshwio.h"
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int int16;
typedef unsigned short int uint16;
typedef signed int long int32;
typedef unsigned long int uint32;
typedef signed long long int int64;
typedef unsigned long long int uint64;
typedef unsigned char boolean;
typedef unsigned int qurt_addr_t;
typedef unsigned int qurt_paddr_t;
typedef unsigned long long qurt_paddr_64_t;
typedef unsigned int qurt_mem_region_t;
typedef unsigned int qurt_mem_fs_region_t;
typedef unsigned int qurt_mem_pool_t;
typedef unsigned int qurt_size_t;
typedef unsigned long long qurt_mmu_entry_t;
typedef enum {
        QURT_MEM_MAPPING_VIRTUAL=0,
        QURT_MEM_MAPPING_PHYS_CONTIGUOUS = 1,
        QURT_MEM_MAPPING_IDEMPOTENT=2,
        QURT_MEM_MAPPING_VIRTUAL_FIXED=3,
        QURT_MEM_MAPPING_NONE=4,
        QURT_MEM_MAPPING_VIRTUAL_RANDOM=7,
        QURT_MEM_MAPPING_INVALID=10,
} qurt_mem_mapping_t;
typedef enum {
        QURT_MEM_CACHE_WRITEBACK=7,
        QURT_MEM_CACHE_NONE_SHARED=6,
        QURT_MEM_CACHE_WRITETHROUGH=5,
        QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE=0,
        QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE=1,
        QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE=QURT_MEM_CACHE_WRITEBACK,
        QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE=QURT_MEM_CACHE_WRITETHROUGH,
        QURT_MEM_CACHE_DEVICE = 4,
        QURT_MEM_CACHE_NONE = 4,
        QURT_MEM_CACHE_DEVICE_SFC = 2,
        QURT_MEM_CACHE_INVALID=10,
} qurt_mem_cache_mode_t;
typedef enum {
        QURT_PERM_READ=0x1,
        QURT_PERM_WRITE=0x2,
        QURT_PERM_EXECUTE=0x4,
        QURT_PERM_FULL=QURT_PERM_READ|QURT_PERM_WRITE|QURT_PERM_EXECUTE,
} qurt_perm_t;
typedef enum {
        QURT_MEM_ICACHE,
        QURT_MEM_DCACHE
} qurt_mem_cache_type_t;
typedef enum {
    QURT_MEM_CACHE_FLUSH,
    QURT_MEM_CACHE_INVALIDATE,
    QURT_MEM_CACHE_FLUSH_INVALIDATE,
    QURT_MEM_CACHE_FLUSH_ALL,
    QURT_MEM_CACHE_FLUSH_INVALIDATE_ALL,
    QURT_MEM_CACHE_TABLE_FLUSH_INVALIDATE,
} qurt_mem_cache_op_t;
typedef enum {
        QURT_MEM_REGION_LOCAL=0,
        QURT_MEM_REGION_SHARED=1,
        QURT_MEM_REGION_USER_ACCESS=2,
        QURT_MEM_REGION_FS=4,
        QURT_MEM_REGION_INVALID=10,
} qurt_mem_region_type_t;
struct qurt_pgattr {
   unsigned pga_value;
};
typedef struct qurt_pgattr qurt_pgattr_t;
typedef struct {
    qurt_mem_mapping_t mapping_type;
    unsigned char perms;
    unsigned short owner;
    qurt_pgattr_t pga;
    unsigned ppn;
    qurt_addr_t virtaddr;
    qurt_mem_region_type_t type;
    qurt_size_t size;
} qurt_mem_region_attr_t;
typedef struct {
    char name[32];
    struct ranges{
        unsigned int start;
        unsigned int size;
    } ranges[16];
} qurt_mem_pool_attr_t;
typedef enum {
    HEXAGON_L1_I_CACHE = 0,
    HEXAGON_L1_D_CACHE = 1,
    HEXAGON_L2_CACHE = 2
} qurt_cache_type_t;
typedef enum {
    FULL_SIZE = 0,
    HALF_SIZE = 1,
    THREE_QUARTER_SIZE = 2,
    SEVEN_EIGHTHS_SIZE = 3
} qurt_cache_partition_size_t;
static inline unsigned qurt_mmu_entry_get_vaddr (qurt_mmu_entry_t entry)
{
    return ((((unsigned long long)entry >> 32) & 0xFFFFF) << 12);
}
static inline unsigned qurt_mmu_entry_is_writeable (qurt_mmu_entry_t entry)
{
    return ((unsigned long long)entry & (1<<30)) != 0;
}
static inline unsigned qurt_mmu_entry_is_device_type (qurt_mmu_entry_t entry)
{
    return ( ((((unsigned long long)entry >> 23) & 0xF) == QURT_MEM_CACHE_DEVICE) ||
             ((((unsigned long long)entry >> 23) & 0xF) == QURT_MEM_CACHE_DEVICE_SFC) );
}
static inline unsigned qurt_mmu_entry_get_size (unsigned long long ent)
{
    unsigned tmp;
    tmp = ent & 0x7F;
    return (((tmp & -tmp) * (tmp & -tmp)) << 12);
}
static inline int qurt_mmu_entry_cmp_by_vaddr (const void *a, const void *b)
{
   return ( (int)(((unsigned long long)*(const qurt_mmu_entry_t *)a >> 32) & 0xFFFFF) -
            (int)(((unsigned long long)*(const qurt_mmu_entry_t *)b >> 32) & 0xFFFFF) );
}
typedef int ADSPResult;
typedef long _Int32t;
typedef unsigned long _Uint32t;
typedef int _Ptrdifft;
typedef unsigned int _Sizet;
typedef __builtin_va_list va_list;
typedef long long _Longlong;
typedef unsigned long long _ULonglong;
typedef int _Wchart;
typedef int _Wintt;
typedef va_list _Va_list;
void _Atexit(void (*)(void));
typedef char _Sysch_t;
void _Locksyslock(int);
void _Unlocksyslock(int);
typedef unsigned short __attribute__((__may_alias__)) alias_short;
static alias_short *strict_aliasing_workaround(unsigned short *ptr) __attribute__((always_inline,unused));
static alias_short *strict_aliasing_workaround(unsigned short *ptr)
{
  alias_short *aliasptr = (alias_short *)ptr;
  return aliasptr;
}
typedef signed char int8_t;
typedef short int16_t;
typedef _Int32t int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef _Uint32t uint32_t;
typedef signed char int_least8_t;
typedef short int_least16_t;
typedef _Int32t int_least32_t;
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef _Uint32t uint_least32_t;
typedef signed char int_fast8_t;
typedef short int_fast16_t;
typedef _Int32t int_fast32_t;
typedef unsigned char uint_fast8_t;
typedef unsigned short uint_fast16_t;
typedef _Uint32t uint_fast32_t;
typedef unsigned int uintptr_t;
typedef int intptr_t;
typedef _Longlong int64_t;
typedef _ULonglong uint64_t;
typedef _Longlong int_least64_t;
typedef _ULonglong uint_least64_t;
typedef _Longlong int_fast64_t;
typedef _ULonglong uint_fast64_t;
typedef _Longlong intmax_t;
typedef _ULonglong uintmax_t;
typedef enum
{
   AVTIMER_HW_INSTANCE_INDEX_INVALID = -1,
   AVTIMER_HW_INSTANCE_INDEX_0 = 0,
   AVTIMER_HW_INSTANCE_INDEX_1,
   AVTIMER_HW_INSTANCE_INDEX_2,
   AVTIMER_HW_INSTANCE_INDEX_MAX,
} avtimer_hw_index;
typedef enum
{
   AVTIMER_HW_MODE_INVALID = -1,
   AVTIMER_HW_MODE_N_ALPAH = 0,
   AVTIMER_HW_MODE_L_MN,
   AVTIMER_HW_MODE_QCOUNTER,
   AVTIMER_HW_MODE_MAX = (0x7FFFFFFF)
} avtimer_hw_mode;
typedef struct
{
  uint32_t offset_addr;
  avtimer_hw_mode mode;
}avtimer_instance_info_t;
enum
{
  TIMESTAMP_LATCH_TYPE_SOFTWARE = 0,
  TIMESTAMP_LATCH_TYPE_HARDWARE
};
enum
{
  PRIMARY_MI2S = 0,
  PRIMARY_PCM = 0,
  PRIMARY_TDM = 0,
  PRIMARY_MUX = 0,
  SECONDARY_MI2S = 1,
  SECONDARY_PCM = 1,
  SECONDARY_TDM = 1,
  SECONDARY_MUX = 1,
  TERTIARY_MI2S = 2,
  TERTIARY_PCM = 2,
  TERTIARY_TDM = 2,
  TERTIARY_MUX = 2,
  QUATERNARY_MI2S = 3,
  QUATERNARY_PCM = 3,
  QUATERNARY_TDM = 3,
  QUATERNARY_MUX = 3,
  SPEAKER_I2S = 4,
  QUINARY_MI2S = 5,
  QUINARY_PCM = 4,
  QUINARY_TDM = 4,
  QUINARY_MUX = 4,
  SENARY_MI2S = 6,
  INT_MI2S_0 = 7,
  INT_MI2S_1 = 8,
  INT_MI2S_2 = 9,
  INT_MI2S_3 = 10,
  INT_MI2S_4 = 11,
  INT_MI2S_5 = 12,
  INT_MI2S_6 = 13
};
typedef enum lpasshwio_irq_type
{
   IRQ_TYPE_UNSUPPORTED = 0,
   IRQ_TYPE_DEFAULT = 1,
   IRQ_TYPE_PMU = 2,
   IRQ_TYPE_PDC = 3,
   IRQ_TYPE_MAX = 0xFFFFFFFF
}lpasshwio_irq_type_t;
typedef enum lpasshwio_gpio_type
{
   GPIO_TYPE_UNSUPPORTED = 0,
   GPIO_TYPE_DEFAULT = 1,
   GPIO_TYPE_LPI = 2,
   GPIO_TYPE_PDC = 3,
   GPIO_TYPE_MAX = 0xFFFFFFFF
}lpasshwio_gpio_type_t;
enum
{
  DATA_LINE_SINK = 1,
  DATA_LINE_SOURCE = 2,
  DATA_LINE_BIDIRECT = 3,
};
enum
{
  NOT_SUPPORTED = 0,
  SUPPORTED = 1,
};
typedef struct
{
   uint32 hw_ver_reg_base;
   lpasshwio_gpio_type_t codec_intr_gpio_type;
   lpasshwio_irq_type_t codec_intr_type;
   uint32 gpio_num_cdc_int;
}HwdLpassPropertyType;
typedef struct
{
   uint32 hdmi_reg_base;
   uint32 hdmi_reg_size;
   uint32 is_supported;
} lpasshwio_prop_hdmi14_struct_t;
typedef struct
{
  uint32_t intf_idx;
  uint32_t inf_type_data_line_caps;
  uint32_t inf_type_direction;
  uint32_t num_ports;
  uint32_t *port_list;
} lpasshwio_intf_port_info_t;
typedef struct
{
  uint32_t i2s_reg_offset_addr;
  uint32_t int_i2s_reg_offset_addr;
  uint32_t inf_type_cnt;
  lpasshwio_intf_port_info_t *intf_port_info[14];
  uint32_t is_supported;
} lpasshwio_prop_i2s_struct_t;
typedef struct
{
   uint32 pcm_reg_offset_addr;
   uint32 inf_type_cnt;
   uint32 inf_type_list[5];
   uint32 inf_type_data_line_caps[5];
   uint32 inf_type_direction[5];
   uint32 is_supported;
} lpasshwio_prop_pcm_struct_t;
typedef struct
{
   uint32 tdm_reg_offset_addr;
   uint32 inf_type_cnt;
   uint32 inf_type_list[5];
   uint32 inf_type_data_line_caps[5];
   uint32 inf_type_direction[5];
   uint32 is_supported;
} lpasshwio_prop_tdm_struct_t;
typedef struct
{
   uint32 mux_reg_offset_addr;
   uint32 mux_inf_type_cnt;
   uint32 shared_inf_type_list_map[5];
   uint32 is_supported;
} lpasshwio_prop_lpaif_mux_struct_t;
typedef struct
{
   uint32 lpaif_reg_base;
   uint32 lpaif_reg_size;
  lpasshwio_prop_i2s_struct_t i2s_prop;
  lpasshwio_prop_pcm_struct_t pcm_prop;
  lpasshwio_prop_tdm_struct_t tdm_prop;
  lpasshwio_prop_hdmi14_struct_t hdmi_prop;
  lpasshwio_prop_lpaif_mux_struct_t lpaif_mux_prop;
   uint32 hw_revision;
} lpasshwio_prop_lpaif_struct_t;
typedef struct
{
   uint32 dma_reg_base;
   uint32 dma_reg_size;
   uint32 dma_int_reg_offset_addr;
   uint32 rddma_reg_offset_addr;
   uint32 wrdma_reg_offset_addr;
   uint32 stc_rddma_reg_offset_addr;
   uint32 stc_wrdma_reg_offset_addr;
   uint32 wrdma5_reg_offset_addr;
   uint32 stc_wrdma5_reg_offset_addr;
   uint32 sink_dma_cnt;
   uint32 source_dma_cnt;
   uint32 int_irq_no;
   uint32 int_output_index;
   uint32 hw_revision;
} lpasshwio_prop_afe_dma_struct_t;
typedef struct
{
   uint32 tcsrRegBase;
   uint32 tcsrRegSize;
   uint32 hw_revision;
   uint32 clk_mgr_coreid;
   uint32 supportedClkIdCnt;
   uint32 supportedClkId[43];
   uint32 infPcmOeCnt;
   uint32 infPcmOeSupportedClkId[11];
   uint32 infPcmOeClkIdToUse[11];
   uint32 infPcmOeClkFreq[11];
} HwdGenericClkPropertyType;
typedef struct
{
   uint32 baseRegAddr;
   uint32 regSize;
  avtimer_instance_info_t avtimer_instance_info[AVTIMER_HW_INSTANCE_INDEX_MAX];
   uint32 audsync_offset_addr;
   uint32 isrHookPinNum;
   uint32 hw_revision;
} HwdAvtimerPropertyType;
typedef struct
{
   uint32 baseRegAddr;
   uint32 regSize;
   uint32 isrHookPinNum;
   uint32 hw_revision;
} HwdResamplerPropertyType;
typedef struct vfr_src_prop
{
   uint32 irq_mux_ctl_sel;
   uint32 is_supported;
} vfr_src_prop_t;
typedef struct
{
   uint32 reg_base_addr;
   uint32 reg_size;
   uint32 vfr_ts_mux_offset1_addr;
   uint32 gp_ts_mux_offset0_addr;
   uint32 gp_ts_mux_offset1_addr;
   uint32 hw_revision;
   uint32 hw_latch_ver;
   uint32 num_vfr_ts_mux;
   uint32 num_gp_ts_mux;
   uint32 l2vic_num[2];
  vfr_src_prop_t vfr_src_prop[2];
   lpasshwio_irq_type_t vfr_irq_type;
}lpasshwio_vfr_prop_t;
typedef struct
{
   uint32 hw_revision;
   uint32 ccu_mb0_ctrl_reg_phy_addr;
   uint32 q6ss_ipc_reg_phy_addr;
   uint32 q6ss_ipc_reg_ack_bit_pos;
   uint16 ipc_send_mail_box_offset;
   uint16 internal_bt_tx_int;
   uint16 internal_bt_rx_int;
   uint16 internal_fm_tx_rx_int;
   uint16 internal_bt_fm_ipc_int;
   uint16 hw_timestamp_available;
}lpasshwio_prop_riva_struct_t;
typedef struct
{
   uint32 is_multi_endpoint;
   uint32 max_eps_per_master[2];
}sb_multi_ep_info_t;
typedef struct
{
   uint32 hw_revision;
   uint32 avtimer_latch_type;
  sb_multi_ep_info_t sb_multi_ep_info[2];
}lpasshwio_prop_slimbus_struct_t;
typedef struct
{
   uint32 base_reg_phy_addr;
   uint32 reg_size;
   uint32 reset_reg_phy_addr;
   uint32 reset_reg_size;
   uint32 int_irq_no;
   uint32 num_hw_buf;
   uint32 hw_revision;
} lpasshwio_prop_spdif_struct_t;
typedef struct
{
   uint32 lpass_reg_base_addr;
   uint32 lpass_reg_size;
   uint32 lpass_reset_reg_base_addr;
   uint32 lpass_reset_reg_size;
   uint32 hdmi_core_reg_base_addr;
   uint32 hdmi_core_reg_size;
   uint32 hw_revision;
} lpasshwio_prop_hdmi_output_struct_t;
typedef struct {
  uint64 base_phy_addr;
   uint32 size;
   uint32 size_afe_dma_buf;
   uint32 size_afe_working_buf;
   uint32 cache_attribute;
   uint32 ahb_voting_scale_factor;
} lpm_prop_struct_t;
typedef unsigned char byte;
typedef unsigned long int bool32;
typedef enum {
    MMPM_STATUS_SUCCESS,
    MMPM_STATUS_FAILED,
    MMPM_STATUS_NOMEMORY,
    MMPM_STATUS_VERSIONNOTSUPPORT,
    MMPM_STATUS_BADCLASS,
    MMPM_STATUS_BADSTATE,
    MMPM_STATUS_BADPARM,
    MMPM_STATUS_INVALIDFORMAT,
    MMPM_STATUS_UNSUPPORTED,
    MMPM_STATUS_RESOURCENOTFOUND,
    MMPM_STATUS_BADMEMPTR,
    MMPM_STATUS_BADHANDLE,
    MMPM_STATUS_RESOURCEINUSE,
    MMPM_STATUS_NOBANDWIDTH,
    MMPM_STATUS_NULLPOINTER,
    MMPM_STATUS_NOTINITIALIZED,
    MMPM_STATUS_RESOURCENOTREQUESTED,
    MMPM_STATUS_CORERESOURCENOTAVAILABLE,
    MMPM_STATUS_MAX,
    MMPM_STATUS_FORCE32BITS = 0x7FFFFFFF
} MmpmStatusType;
typedef MmpmStatusType MMPM_STATUS;
typedef enum {
    MMPM_CORE_ID_NONE = 0,
    MMPM_CORE_ID_2D_GRP = 1,
    MMPM_CORE_ID_3D_GRP = 2,
    MMPM_CORE_ID_MDP = 3,
    MMPM_CORE_ID_VCODEC = 4,
    MMPM_CORE_ID_VPE = 5,
    MMPM_CORE_ID_VFE = 6,
    MMPM_CORE_ID_MIPICSI = 7,
    MMPM_CORE_ID_SENSOR = 8,
    MMPM_CORE_ID_JPEGD = 9,
    MMPM_CORE_ID_JPEGE = 10,
    MMPM_CORE_ID_FABRIC = 11,
    MMPM_CORE_ID_IMEM = 12,
    MMPM_CORE_ID_SMMU = 13,
    MMPM_CORE_ID_ROTATOR = 14,
    MMPM_CORE_ID_TV = 15,
    MMPM_CORE_ID_DSI = 16,
    MMPM_CORE_ID_AUDIOIF = 17,
    MMPM_CORE_ID_GMEM = 18,
    MMPM_CORE_ID_LPASS_START = 100,
    MMPM_CORE_ID_LPASS_ADSP = 101,
    MMPM_CORE_ID_LPASS_CORE = 102,
    MMPM_CORE_ID_LPASS_LPM = 103,
    MMPM_CORE_ID_LPASS_DML = 104,
    MMPM_CORE_ID_LPASS_AIF = 105,
    MMPM_CORE_ID_LPASS_SLIMBUS = 106,
    MMPM_CORE_ID_LPASS_MIDI = 107,
    MMPM_CORE_ID_LPASS_AVSYNC = 108,
    MMPM_CORE_ID_LPASS_HWRSMP = 109,
    MMPM_CORE_ID_LPASS_SRAM = 110,
    MMPM_CORE_ID_LPASS_DCODEC = 111,
    MMPM_CORE_ID_LPASS_SPDIF = 112,
    MMPM_CORE_ID_LPASS_HDMIRX = 113,
    MMPM_CORE_ID_LPASS_HDMITX = 114,
    MMPM_CORE_ID_LPASS_SIF = 115,
    MMPM_CORE_ID_LPASS_BSTC = 116,
    MMPM_CORE_ID_LPASS_ADSP_HVX = 117,
    MMPM_CORE_ID_LPASS_END,
    MMPM_CORE_ID_MAX,
    MMPM_CORE_ID_FORCE32BITS = 0x7FFFFFFF
} MmpmCoreIdType;
typedef enum {
    MMPM_CALLBACK_EVENT_ID_NONE,
    MMPM_CALLBACK_EVENT_ID_IDLE,
    MMPM_CALLBACK_EVENT_ID_BUSY,
    MMPM_CALLBACK_EVENT_ID_THERMAL,
    MMPM_CALLBACK_EVENT_ID_COMPLETE,
    MMPM_CALLBACK_EVENT_ID_MAX,
    MMPM_CALLBACK_EVENT_ID_FORCE32BITS = 0x7FFFFFFF
} MmpmCallbackEventIdType;
typedef struct {
    MmpmCallbackEventIdType eventId;
    uint32 clientId;
    uint32 callbackDataSize;
    void* callbackData;
} MmpmCallbackParamType;
typedef struct {
    uint32 reqTag;
    MMPM_STATUS result;
}MmpmCompletionCallbackDataType;
typedef enum{
    MMPM_CORE_INSTANCE_NONE,
    MMPM_CORE_INSTANCE_0,
    MMPM_CORE_INSTANCE_1,
    MMPM_CORE_INSTANCE_2,
    MMPM_CORE_INSTANCE_MAX,
    MMPM_CORE_INSTANCE_FORCE32BITS = 0x7FFFFFFF
} MmpmCoreInstanceIdType;
typedef struct {
    uint32 rev;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    char *pClientName;
    uint32 pwrCtrlFlag;
    uint32 callBackFlag;
    uint32 (*MMPM_Callback)(MmpmCallbackParamType *pCbParam);
    uint32 cbFcnStackSize;
} MmpmRegParamType;
typedef enum {
    MMPM_RSC_ID_NONE = 0,
    MMPM_RSC_ID_POWER = 1,
    MMPM_RSC_ID_VREG = 2,
    MMPM_RSC_ID_REG_PROG = 3,
    MMPM_RSC_ID_CORE_CLK = 4,
    MMPM_RSC_ID_CORE_CLK_DOMAIN = 5,
    MMPM_RSC_ID_MEM_BW = 6,
    MMPM_RSC_ID_AXI_EN = 7,
    MMPM_RSC_ID_MIPS = 8,
    MMPM_RSC_ID_SLEEP_LATENCY = 9,
    MMPM_RSC_ID_ACTIVE_STATE = 10,
    MMPM_RSC_ID_PMIC_GPIO = 11,
    MMPM_RSC_ID_RESET = 12,
    MMPM_RSC_ID_MIPS_EXT = 13,
    MMPM_RSC_ID_GENERIC_BW = 14,
    MMPM_RSC_ID_THERMAL = 15,
    MMPM_RSC_ID_MEM_POWER = 16,
    MMPM_RSC_ID_GENERIC_BW_EXT = 17,
    MMPM_RSC_ID_MPPS = 18,
    MMPM_RSC_ID_MAX ,
    MMPM_RSC_ID_FORCE32BITS = 0x7FFFFFFF
} MmpmRscIdType;
typedef enum {
    MMPM_REG_PROG_NONE,
    MMPM_REG_PROG_NORM,
    MMPM_REG_PROG_FAST,
    MMPM_REG_PROG_MAX,
    MMPM_REG_PROG_FORCE32BITS = 0x7FFFFFFF
} MmpmRegProgMatchType;
typedef enum {
    MMPM_CLK_ID_2D_GRP_NONE,
    MMPM_CLK_ID_2D_GRP,
    MMPM_CLK_ID_2D_GRP_MAX,
    MMPM_CLK_ID_2D_GRP_FORCE32BITS = 0x7FFFFFFF
} MmpmClkId2dGrpType;
typedef enum {
    MMPM_CLK_ID_3D_GRP_NONE,
    MMPM_CLK_ID_3D_GRP,
    MMPM_CLK_ID_3D_GRP_MAX,
    MMPM_CLK_ID_3D_GRP_FORCE32BITS = 0x7FFFFFFF
} MmpmClkId3dGrpType;
typedef enum {
    MMPM_CLK_ID_MDP_NONE,
    MMPM_CLK_ID_MDP,
    MMPM_CLK_ID_MDP_VSYNC,
    MMPM_CLK_ID_MDP_LUT,
    MMPM_CLK_ID_MDP_MAX,
    MMPM_CLK_ID_MDP_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdMdpType;
typedef enum {
    MMPM_CLK_ID_VCODEC_NONE,
    MMPM_CLK_ID_VCODEC,
    MMPM_CLK_ID_VCODEC_MAX,
    MMPM_CLK_ID_VCODEC_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdVcodecType;
typedef enum {
    MMPM_CLK_ID_VPE_NONE,
    MMPM_CLK_ID_VPE,
    MMPM_CLK_ID_VPE_MAX,
    MMPM_CLK_ID_VPE_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdVpeType;
typedef enum {
    MMPM_CLK_ID_VFE_NONE,
    MMPM_CLK_ID_VFE,
    MMPM_CLK_ID_VFE_MAX,
    MMPM_CLK_ID_VFE_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdVfeType;
typedef enum {
    MMPM_CLK_ID_CSI_NONE,
    MMPM_CLK_ID_CSI,
    MMPM_CLK_ID_CSI_VFE,
    MMPM_CLK_ID_CSI_PHY,
    MMPM_CLK_ID_CSI_PHY_TIMER,
    MMPM_CLK_ID_CSI_PIX0,
    MMPM_CLK_ID_CSI_PIX1,
    MMPM_CLK_ID_CSI_RDI0,
    MMPM_CLK_ID_CSI_RDI1,
    MMPM_CLK_ID_CSI_RDI2,
    MMPM_CLK_ID_CSI_MAX,
    MMPM_CLK_ID_CSI_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdCsiType;
typedef enum {
    MMPM_CLK_ID_SENSOR_NONE,
    MMPM_CLK_ID_SENSOR,
    MMPM_CLK_ID_SENSOR_MAX,
    MMPM_CLK_ID_SENSOR_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdSensorType;
typedef enum {
    MMPM_CLK_ID_JPEGD_NONE,
    MMPM_CLK_ID_JPEGD,
    MMPM_CLK_ID_JPEGD_MAX,
    MMPM_CLK_ID_JPEGD_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdJpegdType;
typedef enum {
    MMPM_CLK_ID_JPEGE_NONE,
    MMPM_CLK_ID_JPEGE,
    MMPM_CLK_ID_JPEGE_MAX,
    MMPM_CLK_ID_JPEGE_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdJpegeType;
typedef enum {
    MMPM_CLK_ID_ROTATOR_NONE,
    MMPM_CLK_ID_ROTATOR,
    MMPM_CLK_ID_ROTATOR_MAX,
    MMPM_CLK_ID_ROTATOR_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdRotatorType;
typedef enum {
    MMPM_CLK_ID_TV_NONE,
    MMPM_CLK_ID_TV_ENC,
    MMPM_CLK_ID_TV_DAC,
    MMPM_CLK_ID_TV_MDP,
    MMPM_CLK_ID_TV_HDMI_APP,
    MMPM_CLK_ID_TV_HDMI,
    MMPM_CLK_ID_TV_RGB,
    MMPM_CLK_ID_TV_NPL,
    MMPM_CLK_ID_TV_MAX,
    MMPM_CLK_ID_TV_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdTvType;
typedef enum {
    MMPM_CLK_ID_DSI_NONE,
    MMPM_CLK_ID_DSI,
    MMPM_CLK_ID_DSI_ESC,
    MMPM_CLK_ID_DSI_PIX,
    MMPM_CLK_ID_DSI_BYTE,
    MMPM_CLK_ID_DSI_LVDS,
    MMPM_CLK_ID_DSI_MDP_P2,
    MMPM_CLK_ID_DSI_MAX,
    MMPM_CLK_ID_DSI_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdDsiType;
typedef enum {
    MMPM_CLK_ID_AUDIOIF_NONE,
    MMPM_CLK_ID_AUDIOIF_PCM,
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_SPKR_MCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_SPKR_SCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_SPKR_MCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_SPKR_SCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_MIC_MCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_MIC_SCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_MIC_MCLK,
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_MIC_SCLK,
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_MCLK,
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_SCLK,
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_TX_MCLK,
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_TX_SCLK,
    MMPM_CLK_ID_AUDIOIF_LPASS_SB_REF_CLK,
    MMPM_CLK_ID_AUDIOIF_SPS_SB_REF_CLK,
    MMPM_CLK_ID_AUDIOIF_MAX,
    MMPM_CLK_ID_AUDIOIF_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdAudioIfType;
typedef enum {
    MMPM_CLK_ID_LPASS_NONE = 0,
    MMPM_CLK_ID_LPASS_HWRSP_CORE,
    MMPM_CLK_ID_LPASS_MIDI_CORE,
    MMPM_CLK_ID_LPASS_AVSYNC_XO,
    MMPM_CLK_ID_LPASS_AVSYNC_BT,
    MMPM_CLK_ID_LPASS_AVSYNC_FM,
    MMPM_CLK_ID_LPASS_SLIMBUS_CORE,
    MMPM_CLK_ID_LPASS_SLIMBUS2_CORE,
    MMPM_CLK_ID_LPASS_AVTIMER_CORE,
    MMPM_CLK_ID_LPASS_ATIME_CORE,
    MMPM_CLK_ID_LPASS_ATIME2_CORE,
    MMPM_CLK_ID_LPASS_BSTC_CORE,
    MMPM_CLK_ID_LPASS_ADSP_CORE,
    MMPM_CLK_ID_LPASS_AHB_ROOT,
    MMPM_CLK_ID_LPASS_ENUM_MAX,
    MMPM_CLK_ID_LPASS_FORCE32BITS = 0x7fffffff
} MmpmClkIdLpassType;
typedef enum {
    MMPM_CLK_ID_VCAP_NONE,
    MMPM_CLK_ID_VCAP,
    MMPM_CLK_ID_VCAP_NPL,
    MMPM_CLK_ID_VCAP_MAX,
    MMPM_CLK_ID_VCAP_FORCE32BITS = 0x7FFFFFFF
} MmpmClkIdVcapType;
typedef union {
    MmpmClkId2dGrpType clkId2dGrp;
    MmpmClkId3dGrpType clkId3dGrp;
    MmpmClkIdMdpType clkIdMdp;
    MmpmClkIdVcodecType clkIdVcodec;
    MmpmClkIdVpeType clkIdVpe;
    MmpmClkIdVfeType clkIdVfe;
    MmpmClkIdCsiType clkIdCsi;
    MmpmClkIdSensorType clkIdSensor;
    MmpmClkIdJpegdType clkIdJpegd;
    MmpmClkIdJpegeType clkIdJpege;
    MmpmClkIdRotatorType clkIdRotator;
    MmpmClkIdTvType clkIdTv;
    MmpmClkIdDsiType clkIdDsi;
    MmpmClkIdAudioIfType clkIdAudioIf;
    MmpmClkIdVcapType clkIdVcap;
    MmpmClkIdLpassType clkIdLpass;
} MmpmCoreClkIdType;
typedef enum {
    MMPM_FREQ_AT_LEAST,
    MMPM_FREQ_AT_MOST,
    MMPM_FREQ_CLOSEST,
    MMPM_FREQ_EXACT,
    MMPM_FREQ_MAX,
    MMPM_FREQ_FORCE32BITS = 0x7FFFFFFF
} MmpmFreqMatchType;
typedef struct {
    MmpmCoreClkIdType clkId;
    uint32 clkFreqHz;
    MmpmFreqMatchType freqMatch;
} MmpmClkValType;
typedef struct {
    uint32 numOfClk;
    MmpmClkValType *pClkArray;
} MmpmClkReqType;
typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_NONE,
    MMPM_CLK_DOMAIN_SRC_ID_HDMI0,
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_MAX,
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_FORCE32BITS = 0x7FFFFFFF
} MmpmClkDomainSrcIdHdmiType;
typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_DSI_NONE,
    MMPM_CLK_DOMAIN_SRC_ID_DSI0,
    MMPM_CLK_DOMAIN_SRC_ID_DSI1,
    MMPM_CLK_DOMAIN_SRC_ID_LVDS,
    MMPM_CLK_DOMAIN_SRC_ID_DSI_MAX,
    MMPM_CLK_DOMAIN_SRC_ID_DSI_FORCE32BITS = 0x7FFFFFFF
} MmpmClkDomainSrcIdDsiType;
typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_NONE,
    MMPM_CLK_DOMAIN_SRC_ID_Q6PLL,
    MMPM_CLK_DOMAIN_SRC_ID_AUDIOPLL,
    MMPM_CLK_DOMAIN_SRC_ID_PRIUSPLL,
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX,
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_FORCE32BITS = 0x7FFFFFFF
} MmpmClkDomainSrcIdLpassType;
typedef union {
    MmpmClkDomainSrcIdHdmiType clkDomainSrcIdHdmi;
    MmpmClkDomainSrcIdDsiType clkDomainSrcIdDsi;
    MmpmClkDomainSrcIdLpassType clkDomainSrcIdLpass;
} MmpmClkDomainSrcIdType;
typedef struct {
    MmpmCoreClkIdType clkId;
    uint32 M_val;
    uint32 N_val;
    uint32 n2D_val;
    uint32 div_val;
    uint32 clkFreqHz;
    MmpmClkDomainSrcIdType clkDomainSrc;
} MmpmClkDomainType;
typedef struct {
    uint32 numOfClk;
    MmpmClkDomainType *pClkDomainArray;
} MmpmClkDomainReqType;
typedef enum {
    MMPM_THERMAL_NONE,
    MMPM_THERMAL_LOW,
    MMPM_THERMAL_NORM,
    MMPM_THERMAL_HIGH_L1,
    MMPM_THERMAL_HIGH_L2,
    MMPM_THERMAL_HIGH_L3,
    MMPM_THERMAL_HIGH_L4,
    MMPM_THERMAL_HIGH_L5,
    MMPM_THERMAL_HIGH_L6,
    MMPM_THERMAL_HIGH_L7,
    MMPM_THERMAL_HIGH_L8,
    MMPM_THERMAL_HIGH_L9,
    MMPM_THERMAL_HIGH_L10,
    MMPM_THERMAL_HIGH_L11,
    MMPM_THERMAL_HIGH_L12,
    MMPM_THERMAL_HIGH_L13,
    MMPM_THERMAL_HIGH_L14,
    MMPM_THERMAL_HIGH_L15,
    MMPM_THERMAL_HIGH_L16,
    MMPM_THERMAL_MAX,
    MMPM_THERMAL_FORCE32BITS = 0x7FFFFFFF
} MmpmThermalType;
typedef enum {
    MMPM_BW_USAGE_2D_GRP_NONE,
    MMPM_BW_USAGE_2D_GRP,
    MMPM_BW_USAGE_2D_GRP_MAX,
    MMPM_BW_USAGE_2D_GRP_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsage2dGrpType;
typedef enum {
    MMPM_BW_USAGE_3D_GRP_NONE,
    MMPM_BW_USAGE_3D_GRP,
    MMPM_BW_USAGE_3D_GRP_MAX,
    MMPM_BW_USAGE_3D_GRP_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsage3dGrpType;
typedef enum {
    MMPM_BW_USAGE_MDP_NONE,
    MMPM_BW_USAGE_MDP,
    MMPM_BW_USAGE_MDP_HRES,
    MMPM_BW_USAGE_MDP_MAX,
    MMPM_BW_USAGE_MDP_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageMdpType;
typedef enum {
    MMPM_BW_USAGE_VCODEC_NONE,
    MMPM_BW_USAGE_VCODEC_ENC,
    MMPM_BW_USAGE_VCODEC_DEC,
    MMPM_BW_USAGE_VCODEC_ENC_DEC,
    MMPM_BW_USAGE_VCODEC_MAX,
    MMPM_BW_USAGE_VCODEC_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageVcodecType;
typedef enum {
    MMPM_BW_USAGE_VPE_NONE,
    MMPM_BW_USAGE_VPE,
    MMPM_BW_USAGE_VPE_MAX,
    MMPM_BW_USAGE_VPE_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageVpeType;
typedef enum {
    MMPM_BW_USAGE_VFE_NONE,
    MMPM_BW_USAGE_VFE,
    MMPM_BW_USAGE_VFE_MAX,
    MMPM_BW_USAGE_VFE_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageVfeType;
typedef enum {
    MMPM_BW_USAGE_JPEGD_NONE,
    MMPM_BW_USAGE_JPEGD,
    MMPM_BW_USAGE_JPEGD_MAX,
    MMPM_BW_USAGE_JPEGD_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageJpegdType;
typedef enum {
    MMPM_BW_USAGE_JPEGE_NONE,
    MMPM_BW_USAGE_JPEGE,
    MMPM_BW_USAGE_JPEGE_MAX,
    MMPM_BW_USAGE_JPEGE_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageJpegeType;
typedef enum {
    MMPM_BW_USAGE_ROTATOR_NONE,
    MMPM_BW_USAGE_ROTATOR,
    MMPM_BW_USAGE_ROTATOR_MAX,
    MMPM_BW_USAGE_ROTATOR_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageRotatorType;
typedef enum {
    MMPM_BW_USAGE_LPASS_NONE,
    MMPM_BW_USAGE_LPASS_DSP,
    MMPM_BW_USAGE_LPASS_DMA,
    MMPM_BW_USAGE_LPASS_EXT_CPU,
    MMPM_BW_USAGE_LPASS_ENUM_MAX,
    MMPM_BW_USAGE_LPASS_FORCE32BITS = 0x7FFFFFFF
} MmpmBwUsageLpassType;
typedef union {
    MmpmBwUsage2dGrpType bwUsage2dGrp;
    MmpmBwUsage3dGrpType bwUsage3dGrp;
    MmpmBwUsageMdpType bwUsageMdp;
    MmpmBwUsageVcodecType bwUsageVcodec;
    MmpmBwUsageVpeType bwUsageVpe;
    MmpmBwUsageVfeType bwUsageVfe;
    MmpmBwUsageJpegdType bwUsageJpegd;
    MmpmBwUsageJpegeType bwUsageJpege;
    MmpmBwUsageRotatorType bwUsageRotator;
    MmpmBwUsageLpassType bwUsageLpass;
} MmpmCoreBwUsageType;
typedef enum {
    MMPM_BW_PORT_ID_NONE = 0,
    MMPM_BW_PORT_ID_ADSP_MASTER,
    MMPM_BW_PORT_ID_DML_MASTER,
    MMPM_BW_PORT_ID_AIF_MASTER,
    MMPM_BW_PORT_ID_SLIMBUS_MASTER,
    MMPM_BW_PORT_ID_SLIMBUS2_MASTER,
    MMPM_BW_PORT_ID_MIDI_MASTER,
    MMPM_BW_PORT_ID_HWRSMP_MASTER,
    MMPM_BW_PORT_ID_EXT_AHB_MASTER,
    MMPM_BW_PORT_ID_SPDIF_MASTER,
    MMPM_BW_PORT_ID_HDMIRX_MASTER,
    MMPM_BW_PORT_ID_HDMITX_MASTER,
    MMPM_BW_PORT_ID_SIF_MASTER,
    MMPM_BW_PORT_ID_DML_SLAVE,
    MMPM_BW_PORT_ID_AIF_SLAVE,
    MMPM_BW_PORT_ID_SLIMBUS_SLAVE,
    MMPM_BW_PORT_ID_SLIMBUS2_SLAVE,
    MMPM_BW_PORT_ID_MIDI_SLAVE,
    MMPM_BW_PORT_ID_HWRSMP_SLAVE,
    MMPM_BW_PORT_ID_AVSYNC_SLAVE,
    MMPM_BW_PORT_ID_LPM_SLAVE,
    MMPM_BW_PORT_ID_SRAM_SLAVE,
    MMPM_BW_PORT_ID_EXT_AHB_SLAVE,
    MMPM_BW_PORT_ID_DDR_SLAVE,
    MMPM_BW_PORT_ID_OCMEM_SLAVE,
    MMPM_BW_PORT_ID_PERIFNOC_SLAVE,
    MMPM_BW_PORT_ID_SPDIF_SLAVE,
    MMPM_BW_PORT_ID_HDMIRX_SLAVE,
    MMPM_BW_PORT_ID_HDMITX_SLAVE,
    MMPM_BW_PORT_ID_SIF_SLAVE,
    MMPM_BW_PORT_ID_BSTC_SLAVE,
    MMPM_BW_PORT_ID_DCODEC_SLAVE,
    MMPM_BW_PORT_ID_CORE,
    MMPM_BW_PORT_ID_MAX,
    MMPM_BW_PORT_ID_FORCE32BITS = 0x7FFFFFFF
} MmpmBwPortIdType;
typedef struct {
    uint32 memPhyAddr;
    uint32 axiPort;
    uint32 bwBytePerSec;
    uint32 usagePercentage;
    MmpmCoreBwUsageType bwUsageType;
} MmpmBwValType;
typedef struct {
    uint32 numOfBw;
    MmpmBwValType *pBandWidthArray;
} MmpmBwReqType;
typedef struct{
    MmpmBwPortIdType masterPort;
    MmpmBwPortIdType slavePort;
} MmpmBusRouteType;
typedef struct{
    uint64 Ab;
    uint64 Ib;
} MmpmBusBWDataIbAbType;
typedef struct{
    uint64 bwBytePerSec;
    uint32 usagePercentage;
    MmpmBwUsageLpassType usageType;
} MmpmBusBWDataUsageType;
typedef union{
    MmpmBusBWDataIbAbType busBwAbIb;
    MmpmBusBWDataUsageType busBwValue;
} MmpmBusBWDataType;
typedef struct{
    MmpmBusRouteType busRoute;
    MmpmBusBWDataType bwValue;
} MmpmGenBwValType;
typedef struct {
    uint32 numOfBw;
    MmpmGenBwValType *pBandWidthArray;
} MmpmGenBwReqType;
typedef enum {
    MMPM_MEM_POWER_NONE,
    MMPM_MEM_POWER_OFF,
    MMPM_MEM_POWER_RETENTION,
    MMPM_MEM_POWER_ACTIVE,
    MMPM_MEM_POWER_MAX,
    MMPM_MEM_POWER_FORCE32BITS = 0x7FFFFFFF
} MmpmMemPowerStateType;
typedef enum{
    MMPM_MEM_NONE,
    MMPM_MEM_OCMEM,
    MMPM_MEM_LPASS_LPM,
    MMPM_MEM_SRAM,
    MMPM_MEM_MAX,
    MMPM_MEM_FORCE32BITS = 0x7FFFFFFF
} MmpmMemIdType;
typedef struct{
    MmpmMemIdType memory;
    MmpmMemPowerStateType powerState;
}MmpmMemPowerReqParamType;
typedef struct {
    uint32 gpioId;
    boolean configGpio;
    uint32 gpioVoltageSource;
    boolean gpioModeOnOff;
    uint32 gpioModeSelect;
    uint32 gpioOutBufferConfig;
    boolean invertExtPin;
    uint32 gpioCurrentSourcePulls;
    uint32 gpioOutBufferDriveStrength;
    uint32 gpioDtestBufferOnOff;
    uint32 gpioExtPinConfig;
    uint32 gpioSourceConfig;
    boolean interrupPolarity;
    uint32 uartPath;
} MmpmPmicGpioParamType;
typedef enum {
     MMPM_PMIC_CONFIG,
     MMPM_PMIC_CONFIG_BIAS_VOLTAGE,
     MMPM_PMIC_CONFIG_DIGITAL_INPUT,
     MMPM_PMIC_CONFIG_DIGITAL_OUTPUT,
     MMPM_PMIC_CONFIG_SET_VOLTAGE_SOURCE,
     MMPM_PMIC_CONFIG_MODE_SELECTION,
     MMPM_PMIC_CONFIG_SET_OUTPUT_BUFFER,
     MMPM_PMIC_CONFIG_SET_INVERSION,
     MMPM_PMIC_CONFIG_SET_CURRENT_SOURCE_PULLS,
     MMPM_PMIC_CONFIG_SET_EXT_PIN,
     MMPM_PMIC_CONFIG_SET_OUTPUT_BUF_DRIVE_STRG,
     MMPM_PMIC_CONFIG_SET_SOURCE,
     MMPM_PMIC_CONFIG_SET_INT_POLARITY,
     MMPM_PMIC_CONFIG_SET_MUX_CTRL,
} MmpmPmicGpioConfigType;
typedef struct {
    MmpmPmicGpioConfigType configId;
    MmpmPmicGpioParamType gpioParam;
} MmpmPmicGpioReqType;
typedef enum {
     MMPM_RESET_NONE,
     MMPM_RESET_DEASSERT,
     MMPM_RESET_ASSERT,
     MMPM_RESET_PULSE,
     MMPM_RESET_ENUM_MAX,
     MMPM_RESET_ENUM_FORCE32BITS = 0x7FFFFFFF
} MmpmResetType;
typedef struct {
    MmpmCoreClkIdType clkId;
    MmpmResetType resetType;
} MmpmResetParamType;
typedef struct {
    uint32 numOfReset;
    MmpmResetParamType *pResetParamArray;
} MmpmResetReqType;
typedef enum {
    MMPM_MIPS_REQUEST_NONE = 0,
    MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY,
    MMPM_MIPS_REQUEST_CPU_CLOCK_AND_BW,
    MMPM_MIPS_REQUEST_ENUM_MAX,
    MMPM_MIPS_REQUEST_FORCE32BITS = 0x7FFFFFFF
} MmpmMipsRequestFnType;
typedef struct {
    uint32 mipsTotal;
    uint32 mipsPerThread;
    MmpmBwPortIdType codeLocation;
    MmpmMipsRequestFnType reqOperation;
} MmpmMipsReqType;
typedef struct {
    uint32 mppsTotal;
    uint32 adspFloorClock;
} MmpmMppsReqType;
typedef struct {
    uint32 numOfClk;
    MmpmCoreClkIdType *pClkIdArray;
} MmpmClkIdArrayParamType;
typedef union {
    uint32 vregMilliVolt;
    MmpmRegProgMatchType regProgMatch;
    MmpmPmicGpioReqType *pPmicGpio;
    MmpmClkReqType *pCoreClk;
    MmpmClkDomainReqType *pCoreClkDomain;
    MmpmBwReqType *pBwReq;
    MmpmGenBwReqType *pGenBwReq;
    uint32 sleepMicroSec;
    uint32 mips;
    MmpmResetReqType *pResetReq;
    MmpmMipsReqType *pMipsExt;
    MmpmMppsReqType *pMppsReq;
    MmpmThermalType thermalMitigation;
    MmpmMemPowerReqParamType *pMemPowerState;
    MmpmClkIdArrayParamType *pRelClkIdArray;
    MmpmCoreClkIdType gateClkId;
}MmpmRscParamStructType;
typedef struct {
    MmpmRscIdType rscId;
    MmpmRscParamStructType rscParam;
} MmpmRscParamType;
typedef enum{
     MMPM_API_TYPE_NONE,
     MMPM_API_TYPE_SYNC,
     MMPM_API_TYPE_ASYNC,
     MMPM_API_TYPE_ENUM_MAX,
     MMPM_API_TYPE_FORCE32BITS = 0x7FFFFFFF
} MmpmApiType;
typedef struct {
    MmpmApiType apiType;
    uint32 numOfReq;
    MmpmRscParamType *pReqArray;
    MMPM_STATUS *pStsArray;
    uint32 reqTag;
    void *pExt;
} MmpmRscExtParamType;
typedef enum {
    MMPM_INFO_ID_NONE,
    MMPM_INFO_ID_CORE_CLK,
    MMPM_INFO_ID_CORE_CLK_MAX,
    MMPM_INFO_ID_CORE_CLK_MAX_SVS,
    MMPM_INFO_ID_CORE_CLK_MAX_NOMINAL,
    MMPM_INFO_ID_CORE_CLK_MAX_TURBO,
    MMPM_INFO_ID_MIPS_MAX,
    MMPM_INFO_ID_BW_MAX,
    MMPM_INFO_ID_CRASH_DUMP,
    MMPM_INFO_ID_POWER_SUPPORT,
    MMPM_INFO_ID_CLK_FREQ,
    MMPM_INFO_ID_PERFMON,
    MMPM_INFO_ID_PMIC_GPIO,
    MMPM_INFO_ID_SET_DEBUG_LEVEL,
    MMPM_INFO_ID_MMSS_BUS,
    MMPM_INFO_ID_LPASS_BUS,
    MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS,
    MMPM_INFO_ID_MPPS,
    MMPM_INFO_ID_BW_EXT,
    MMPM_INFO_ID_DCVS_STATE,
    MMPM_INFO_ID_EXT_REQ,
    MMPM_INFO_ID_MAX,
    MMPM_INFO_ID_FORCE32BITS = 0x7FFFFFFF
} MmpmInfoIdType;
typedef struct {
    MmpmCoreClkIdType clkId;
    uint32 clkFreqHz;
} MmpmInfoClkType;
typedef struct {
    uint32 clkId;
    uint32 clkFreqHz;
    uint32 forceMeasure;
} MmpmInfoClkFreqType;
typedef struct
{
    uint32 clientClasses;
    uint32 aggregateMpps;
} MmpmInfoMppsType;
typedef struct
{
    MmpmBusRouteType busRoute;
    uint64 totalBw;
} MmpmInfoBwExtType;
typedef struct {
    uint32 masterMeasureArray[7];
    uint32 measurementConfig[7];
    uint32 latencyMasterPort;
    uint32 holdoffTime;
    uint32 triggerMode;
    void *pDataMsg;
    char pFilename[64];
    uint32 axiClockFreq;
    uint32 clockInfo[10];
    uint32 isClockFreqCalc[10];
} MmpmInfoPerfmonType;
typedef struct {
    uint32 gpioModeSelect;
    uint32 gpioVoltageSource;
    uint32 gpioModeOnOff;
    uint32 gpioOutBufferConfig;
    uint32 gpioOutBufferDriveStrength;
    uint32 gpioCurrentSourcePulls;
    uint32 gpioSourceConfig;
    uint32 gpioDtestBufferOnOff;
    uint32 gpioExtPinConfig;
} MmpmPmicGpioStatusType;
typedef struct {
    uint32 gpioId;
    MmpmPmicGpioStatusType gpioSts;
} MmpmPmicGpioInfoType;
typedef struct {
    uint32 clientId;
    char clientName[32];
    char fabClientName[32];
    uint32 uIbBytePerSec;
    uint32 uAbBytePerSec;
} MmpmBwClientType;
typedef struct {
    uint32 coreId;
    uint32 instanceId;
    uint32 axiPort;
    uint32 slaveId;
    uint32 uIbBytePerSec;
    uint32 uAbBytePerSec;
    uint32 numOfClients;
    MmpmBwClientType client[16];
} MmpmMasterSlaveType;
typedef struct {
    uint32 mmFabClkFreq;
    uint32 appsFabClkFreq;
    uint32 sysFabClkFreq;
    uint32 lpassFabClkFreq;
    uint32 ebiClkFreq;
    uint32 mmImemClkFreq;
    uint32 numOfMasterSlave;
    MmpmMasterSlaveType masterSlave[16];
} MmpmFabStatusInfoType;
typedef union {
    MmpmInfoClkType *pInfoClk;
    boolean bInfoPower;
    MmpmInfoPerfmonType *pInfoPerfmon;
    MmpmInfoClkFreqType *pInfoClkFreqType;
    MmpmPmicGpioInfoType *pPmicGpioStatus;
    uint32 infoSetLotLevel;
    MmpmFabStatusInfoType *pFabStatus;
    uint32 mipsValue;
    uint64 bwValue;
    uint32 aggregateClientClass;
    MmpmInfoMppsType *pMppsInfo;
    MmpmInfoBwExtType *pBwExtInfo;
    boolean dcvsState;
    void *pExtInfo;
} MmpmInfoDataStructType;
typedef struct {
    MmpmInfoIdType infoId;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    MmpmInfoDataStructType info;
} MmpmInfoDataType;
typedef enum{
    MMPM_PARAM_ID_NONE = 0,
    MMPM_PARAM_ID_RESOURCE_LIMIT,
    MMPM_PARAM_ID_CLIENT_OCMEM_MAP,
    MMPM_PARAM_ID_MEMORY_MAP,
    MMPM_PARAM_ID_CLIENT_CLASS,
    MMPM_PARAM_ID_L2_CACHE_LINE_LOCK,
    MMPM_PARAM_ID_DCVS_PARTICIPATION,
    MMPM_PARAM_ID_ENUM_MAX,
    MMPM_PARAM_ID_Force32bit = 0x7fffffff
} MmpmParameterIdType;
typedef struct
{
    MmpmMemIdType memory;
    uint64 startAddress;
    uint32 size;
} MmpmMemoryMapType;
typedef struct
{
    void* startAddress;
    uint32 size;
    uint32 throttleBlockSize;
    uint32 throttlePauseUs;
} MmpmL2CacheLineLockParameterType;
typedef enum
{
    MMPM_UNKNOWN_CLIENT_CLASS = 0x00,
    MMPM_AUDIO_CLIENT_CLASS = 0x01,
    MMPM_VOICE_CLIENT_CLASS = 0x02,
    MMPM_COMPUTE_CLIENT_CLASS = 0x04,
    MMPM_STREAMING_1HVX_CLIENT_CLASS = 0x08,
    MMPM_STREAMING_2HVX_CLIENT_CLASS = 0x10,
    MMPM_MAX_CLIENT_CLASS,
} MmpmClientClassType;
typedef enum
{
    MMPM_DCVS_ADJUST_UP_DOWN,
    MMPM_DCVS_ADJUST_ONLY_UP
} MmpmDcvsEnableOptionsType;
typedef struct
{
    boolean enable;
    MmpmDcvsEnableOptionsType enableOpt;
} MmpmDcvsParticipationType;
typedef struct{
    MmpmParameterIdType paramId;
    void* pParamConfig;
} MmpmParameterConfigType;
typedef struct{
    uint32 regionId;
    uint32 numKeys;
    uint32 *pKey;
} MmpmOcmemMapRegionDescType;
typedef struct{
    uint32 numRegions;
    MmpmOcmemMapRegionDescType *pRegions;
} MmpmOcmemMapType;
uint32 MMPM_Init(char * cmd_line);
uint32 MMPM_Register_Ext(MmpmRegParamType *pRegParam);
MMPM_STATUS MMPM_Deregister_Ext(uint32 clientId);
MMPM_STATUS MMPM_Request(uint32 clientId,
                             MmpmRscParamType *pMmpmRscParam);
MMPM_STATUS MMPM_Release(uint32 clientId,
                             MmpmRscParamType *pMmpmRscParam);
MMPM_STATUS MMPM_Pause(uint32 clientId,
                           MmpmRscParamType *pMmpmRscParam);
MMPM_STATUS MMPM_Resume(uint32 clientId,
                            MmpmRscParamType *pMmpmRscParam);
MMPM_STATUS MMPM_Request_Ext(uint32 clientId,
                                 MmpmRscExtParamType *pRscExtParam);
MMPM_STATUS MMPM_Release_Ext(uint32 clientId,
                                 MmpmRscExtParamType *pRscExtParam);
MMPM_STATUS MMPM_Pause_Ext(uint32 clientId,
                               MmpmRscExtParamType *pRscExtParam);
MMPM_STATUS MMPM_Resume_Ext(uint32 clientId,
                                MmpmRscExtParamType *pRscExtParam);
MMPM_STATUS MMPM_GetInfo(MmpmInfoDataType *pInfoData);
MMPM_STATUS MMPM_SetParameter(uint32 clientId, MmpmParameterConfigType *pParamConfig);
typedef char char_t;
typedef unsigned char bool_t;
typedef struct afe_mod_enable_param_t afe_mod_enable_param_t;
struct afe_mod_enable_param_t
{
  uint16_t enable;
  uint16_t reserved;
}
;
typedef struct afe_hw_mad_cfg_param_v1_t afe_hw_mad_cfg_param_v1_t;
struct afe_hw_mad_cfg_param_v1_t
{
  uint32_t hw_mad_minor_version;
  uint16_t audio_sleep_time_ms;
  uint16_t beacon_sleep_time_ms;
    uint16_t us_sleep_time_ms;
  int16_t reserved;
}
;
typedef struct afe_hw_mad_ctrl_param_v1_t afe_hw_mad_ctrl_param_v1_t;
struct afe_hw_mad_ctrl_param_v1_t
{
  uint32_t minor_version;
  uint16_t mad_type;
  uint16_t mad_enable;
}
;
typedef struct afe_loopback_gain_per_path_param_t afe_loopback_gain_per_path_param_t;
struct afe_loopback_gain_per_path_param_t
{
   uint16_t rx_port_id;
   uint16_t gain;
}
;
typedef enum afe_loopback_routing_mode_t
{
    LB_MODE_DEFAULT = 1,
    LB_MODE_SIDETONE,
    LB_MODE_EC_REF_VOICE_AUDIO,
    LB_MODE_EC_REF_VOICE
} afe_loopback_routing_mode_t ;
typedef struct afe_loopback_cfg_v1_t afe_loopback_cfg_v1_t;
struct afe_loopback_cfg_v1_t
{
    uint32_t loopback_cfg_minor_version;
    uint16_t dst_port_id;
    uint16_t routing_mode;
    uint16_t enable;
    uint16_t reserved;
}
;
typedef struct afe_loopback_cfg_v2_t afe_loopback_cfg_v2_t;
struct afe_loopback_cfg_v2_t
{
    uint32_t loopback_cfg_minor_version;
    uint16_t dst_port_id;
    uint16_t routing_mode;
    uint16_t enable;
    uint16_t reserved;
    uint32_t rate_matching_enable;
}
;
typedef struct afe_cmd_remote_lpass_core_hw_vote_request afe_cmd_remote_lpass_core_hw_vote_request;
struct afe_cmd_remote_lpass_core_hw_vote_request
{
   uint32_t hw_block_id;
   char client_name[8];
}
;
typedef struct afe_cmd_rsp_remote_lpass_core_hw_vote_request afe_cmd_rsp_remote_lpass_core_hw_vote_request;
struct afe_cmd_rsp_remote_lpass_core_hw_vote_request
{
   uint32_t client_handle;
}
;
typedef struct afe_cmd_remote_lpass_core_hw_devote_request afe_cmd_remote_lpass_core_hw_devote_request;
struct afe_cmd_remote_lpass_core_hw_devote_request
{
   uint32_t hw_block_id;
   uint32_t client_handle;
}
;
typedef struct afe_service_cmd_register_rt_port_driver_t afe_service_cmd_register_rt_port_driver_t;
struct afe_service_cmd_register_rt_port_driver_t
{
    uint16_t port_id;
    uint16_t reserved;
}
;
typedef struct afe_service_cmd_unregister_rt_port_driver_t afe_service_cmd_unregister_rt_port_driver_t;
struct afe_service_cmd_unregister_rt_port_driver_t
{
    uint16_t port_id;
    uint16_t reserved;
}
;
typedef struct afe_event_rt_proxy_port_status_t afe_event_rt_proxy_port_status_t;
struct afe_event_rt_proxy_port_status_t
{
    uint16_t port_id;
    uint16_t event_type;
}
;
typedef struct afe_port_data_cmd_rt_proxy_port_write_v2_t afe_port_data_cmd_rt_proxy_port_write_v2_t;
struct afe_port_data_cmd_rt_proxy_port_write_v2_t
{
    uint16_t port_id;
    uint16_t reserved;
    uint32_t buffer_address_lsw;
    uint32_t buffer_address_msw;
    uint32_t mem_map_handle;
    uint32_t available_bytes;
}
;
typedef struct afe_port_data_cmd_rt_proxy_port_read_v2_t afe_port_data_cmd_rt_proxy_port_read_v2_t;
struct afe_port_data_cmd_rt_proxy_port_read_v2_t
{
    uint16_t port_id;
    uint16_t reserved;
    uint32_t buffer_address_lsw;
    uint32_t buffer_address_msw;
    uint32_t mem_map_handle;
    uint32_t available_bytes;
}
;
typedef struct afe_param_id_i2s_cfg_t afe_param_id_i2s_cfg_t;
struct afe_param_id_i2s_cfg_t
{
   uint32_t i2s_cfg_minor_version;
    uint16_t bit_width;
    uint16_t channel_mode;
    uint16_t mono_stereo;
    uint16_t ws_src;
    uint32_t sample_rate;
    uint16_t data_format;
    uint16_t reserved;
}
;
typedef struct afe_param_id_pcm_cfg_t afe_param_id_pcm_cfg_t;
struct afe_param_id_pcm_cfg_t
{
   uint32_t pcm_cfg_minor_version;
    uint16_t aux_mode;
    uint16_t sync_src;
    uint16_t frame_setting;
    uint16_t quant_type;
    uint16_t ctrl_data_out_enable;
    uint16_t reserved;
    uint32_t sample_rate;
    uint16_t bit_width;
    uint16_t num_channels;
    uint16_t slot_number_mapping[4];
}
;
typedef struct afe_param_id_tdm_cfg_t afe_param_id_tdm_cfg_t;
struct afe_param_id_tdm_cfg_t
{
   uint32_t tdm_cfg_minor_version;
    uint32_t num_channels;
    uint32_t sample_rate;
    uint32_t bit_width;
    uint16_t data_format;
    uint16_t sync_mode;
    uint16_t sync_src;
    uint16_t nslots_per_frame;
    uint16_t ctrl_data_out_enable;
    uint16_t ctrl_invert_sync_pulse;
    uint16_t ctrl_sync_data_delay;
    uint16_t slot_width;
    uint32_t slot_mask;
}
;
typedef struct afe_param_id_hdmi_multi_chan_audio_cfg_t afe_param_id_hdmi_multi_chan_audio_cfg_t;
struct afe_param_id_hdmi_multi_chan_audio_cfg_t
{
   uint32_t hdmi_cfg_minor_version;
    uint16_t data_type;
   uint16_t channel_allocation;
    uint32_t sample_rate;
    uint16_t bit_width;
   uint16_t reserved;
}
;
typedef struct afe_param_id_usb_audio_cfg_v1_t afe_param_id_usb_audio_cfg_v1_t;
struct afe_param_id_usb_audio_cfg_v1_t
{
   uint32_t minor_version;
    uint32_t sample_rate;
    uint16_t bit_width;
    uint16_t num_channels;
    uint16_t data_format;
   uint16_t reserved;
}
;
typedef struct afe_param_id_usb_audio_dev_params_v1_t afe_param_id_usb_audio_dev_params_v1_t;
struct afe_param_id_usb_audio_dev_params_v1_t
{
   uint32_t minor_version;
   uint32_t usb_device_token;
}
;
typedef struct afe_param_id_usb_audio_dev_lpcm_fmt_v1_t afe_param_id_usb_audio_dev_lpcm_fmt_v1_t;
struct afe_param_id_usb_audio_dev_lpcm_fmt_v1_t
{
  uint32_t minor_version;
  uint32_t media_format;
}
;
typedef struct afe_param_id_spdif_audio_cfg_t afe_param_id_spdif_audio_cfg_t;
struct afe_param_id_spdif_audio_cfg_t
{
   uint32_t spdif_cfg_minor_version;
   uint32_t sample_rate;
   uint16_t data_format;
   uint16_t num_channels;
   uint16_t bit_width;
   uint16_t reserved;
}
;
typedef struct afe_param_id_chstatus_cfg_t afe_param_id_chstatus_cfg_t;
struct afe_param_id_chstatus_cfg_t
{
   uint32_t ch_status_cfg_minor_version;
   uint32_t ch_status_type;
   uint8_t ch_stat_buf[24];
   uint8_t ch_stat_mask[24];
}
;
typedef struct afe_param_id_hdmi_input_cfg_t afe_param_id_hdmi_input_cfg_t;
struct afe_param_id_hdmi_input_cfg_t
{
   uint32_t hdmi_input_cfg_minor_version;
   uint16_t data_type;
   uint16_t num_channels;
   uint32_t sample_rate;
   uint16_t bit_width;
   uint16_t reserved;
}
;
typedef struct afe_param_id_internal_bt_fm_cfg_t afe_param_id_internal_bt_fm_cfg_t;
struct afe_param_id_internal_bt_fm_cfg_t
{
    uint32_t bt_fm_cfg_minor_version;
    uint16_t num_channels;
    uint16_t bit_width;
    uint32_t sample_rate;
}
;
typedef struct afe_param_id_slimbus_cfg_t afe_param_id_slimbus_cfg_t;
struct afe_param_id_slimbus_cfg_t
{
    uint32_t sb_cfg_minor_version;
    uint16_t slimbus_dev_id;
    uint16_t bit_width;
    uint16_t data_format;
    uint16_t num_channels;
    uint8_t shared_channel_mapping[0x8];
    uint32_t sample_rate;
}
;
typedef struct afe_param_id_rt_proxy_port_cfg_t afe_param_id_rt_proxy_port_cfg_t;
struct afe_param_id_rt_proxy_port_cfg_t
{
    uint32_t rt_proxy_cfg_minor_version;
    uint16_t bit_width;
    uint16_t interleaved;
    uint16_t frame_size;
    uint16_t jitter_allowance;
    uint16_t low_water_mark;
    uint16_t high_water_mark;
    uint32_t sample_rate;
    uint16_t num_channels;
    uint16_t reserved;
}
;
typedef struct afe_param_id_pseudo_port_cfg_t afe_param_id_pseudo_port_cfg_t;
struct afe_param_id_pseudo_port_cfg_t
{
    uint32_t pseud_port_cfg_minor_version;
    uint16_t bit_width;
    uint16_t num_channels;
    uint16_t data_format;
    uint16_t timing_mode;
    uint32_t sample_rate;
}
;
typedef struct afe_param_id_ahb_clock_cfg_t afe_param_id_ahb_clock_cfg_t;
struct afe_param_id_ahb_clock_cfg_t
{
    uint32_t ahb_clock_cfg_minor_version;
    uint32_t ahb_clk_enable;
}
;
typedef struct afe_param_id_lpass_core_shared_clk_cfg_t afe_param_id_lpass_core_shared_clk_cfg_t;
struct afe_param_id_lpass_core_shared_clk_cfg_t
{
   uint32_t lpass_core_shared_clk_cfg_minor_version;
   uint32_t enable;
}
;
typedef struct afe_param_id_device_hw_delay_t afe_param_id_device_hw_delay_t;
struct afe_param_id_device_hw_delay_t
{
    uint32_t device_hw_delay_minor_version;
    uint32_t delay_in_us;
}
;
typedef struct afe_port_cmd_device_start_t afe_port_cmd_device_start_t;
struct afe_port_cmd_device_start_t
{
    uint16_t port_id;
    uint16_t reserved;
}
;
typedef struct afe_port_cmd_devices_restart_t afe_port_cmd_devices_restart_t;
struct afe_port_cmd_devices_restart_t
{
    uint16_t rx_port_id;
    uint16_t tx_port_id;
    uint32_t sample_rate;
}
;
typedef struct afe_port_cmd_device_stop_t afe_port_cmd_device_stop_t;
struct afe_port_cmd_device_stop_t
{
    uint16_t port_id;
    uint16_t reserved;
}
;
typedef struct afe_service_cmd_shared_mem_map_regions_t afe_service_cmd_shared_mem_map_regions_t;
struct afe_service_cmd_shared_mem_map_regions_t
{
    uint16_t mem_pool_id;
    uint16_t num_regions;
    uint32_t property_flag;
}
;
struct afe_service_shared_map_region_payload_t
{
    uint32_t shm_addr_lsw;
    uint32_t shm_addr_msw;
    uint32_t mem_size_bytes;
}
;
typedef struct afe_service_cmdrsp_shared_mem_map_regions_t afe_service_cmdrsp_shared_mem_map_regions_t;
struct afe_service_cmdrsp_shared_mem_map_regions_t
{
    uint32_t mem_map_handle;
}
;
typedef struct afe_service_cmd_shared_mem_unmap_regions_t afe_service_cmd_shared_mem_unmap_regions_t;
struct afe_service_cmd_shared_mem_unmap_regions_t
{
    uint32_t mem_map_handle;
}
;
typedef struct afe_port_param_data_v2_t afe_port_param_data_v2_t;
struct afe_port_param_data_v2_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
}
;
typedef struct afe_port_cmd_set_param_v2_t afe_port_cmd_set_param_v2_t;
struct afe_port_cmd_set_param_v2_t
{
   uint16_t port_id;
   uint16_t payload_size;
   uint32_t payload_address_lsw;
   uint32_t payload_address_msw;
   uint32_t mem_map_handle;
}
;
typedef struct afe_port_cmd_set_param_v3_t afe_port_cmd_set_param_v3_t;
struct afe_port_cmd_set_param_v3_t
{
   uint16_t port_id;
   uint16_t reserved;
   uint32_t payload_address_lsw;
   uint32_t payload_address_msw;
   uint32_t mem_map_handle;
   uint32_t payload_size;
}
;
typedef struct afe_port_param_data_v3_t afe_port_param_data_v3_t;
struct afe_port_param_data_v3_t
{
   uint32_t module_id;
   uint16_t instance_id;
   uint16_t reserved;
   uint32_t param_id;
   uint32_t param_size;
}
;
typedef struct afe_port_cmd_get_param_v2_t afe_port_cmd_get_param_v2_t;
struct afe_port_cmd_get_param_v2_t
{
    uint16_t port_id;
    uint16_t payload_size;
    uint32_t payload_address_lsw;
    uint32_t payload_address_msw;
    uint32_t mem_map_handle;
    uint32_t module_id;
    uint32_t param_id;
}
;
typedef struct afe_port_cmdrsp_get_param_v2_t afe_port_cmdrsp_get_param_v2_t;
struct afe_port_cmdrsp_get_param_v2_t
{
    uint32_t status;
}
;
typedef struct afe_port_cmd_get_param_v3_t afe_port_cmd_get_param_v3_t;
struct afe_port_cmd_get_param_v3_t
{
   uint16_t port_id;
    uint16_t reserved;
    uint32_t payload_address_lsw;
    uint32_t payload_address_msw;
    uint32_t mem_map_handle;
    uint32_t module_id;
    uint16_t instance_id;
    uint16_t reserved_2;
    uint32_t param_id;
    uint32_t payload_size;
}
;
typedef struct afe_port_cmdrsp_get_param_v3_t afe_port_cmdrsp_get_param_v3_t;
struct afe_port_cmdrsp_get_param_v3_t
{
    uint32_t status;
}
;
typedef struct afe_algo_log_header_t afe_algo_log_header_t;
struct afe_algo_log_header_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
}
;
typedef struct afe_svc_param_data_t afe_svc_param_data_t;
struct afe_svc_param_data_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
}
;
typedef struct afe_svc_cmd_set_param_t afe_svc_cmd_set_param_t;
struct afe_svc_cmd_set_param_t
{
   uint32_t payload_size;
   uint32_t payload_address_lsw;
   uint32_t payload_address_msw;
   uint32_t mem_map_handle;
}
;
typedef struct afe_svc_cmd_set_param_v2_t afe_svc_cmd_set_param_v2_t;
struct afe_svc_cmd_set_param_v2_t
{
   uint32_t payload_address_lsw;
   uint32_t payload_address_msw;
   uint32_t mem_map_handle;
   uint32_t payload_size;
}
;
typedef struct afe_svc_param_data_v2_t afe_svc_param_data_v2_t;
struct afe_svc_param_data_v2_t
{
   uint32_t module_id;
   uint16_t instance_id;
   uint16_t reserved;
   uint32_t param_id;
   uint32_t param_size;
}
;
typedef struct afe_slimbus_slave_port_cfg_param_v1_t afe_slimbus_slave_port_cfg_param_v1_t;
struct afe_slimbus_slave_port_cfg_param_v1_t
{
   uint32_t minor_version;
    uint16_t slimbus_dev_id;
   uint16_t slave_dev_pgd_la;
   uint16_t slave_dev_intfdev_la;
    uint16_t bit_width;
    uint16_t data_format;
    uint16_t num_chnnels;
    uint16_t slave_port_mapping[0x8];
}
;
typedef struct afe_param_id_clock_set_t afe_param_id_clock_set_t;
struct afe_param_id_clock_set_t
{
    uint32_t clk_set_minor_version;
    uint32_t clk_id;
    uint32_t clk_freq_in_hz;
    uint16_t clk_attri;
    uint16_t clk_root;
    uint32_t enable;
}
;
typedef struct afe_cdc_slimbus_slave_cfg_param_v1_t afe_cdc_slimbus_slave_cfg_param_v1_t;
struct afe_cdc_slimbus_slave_cfg_param_v1_t
{
    uint32_t slave_dev_minor_version;
    uint32_t device_enum_addr_lsw;
    uint32_t device_enum_addr_msw;
    uint16_t tx_slave_port_offset;
    uint16_t rx_slave_port_offset;
}
;
typedef struct afe_cdc_reg_cfg_param_v1_t afe_cdc_reg_cfg_param_v1_t;
struct afe_cdc_reg_cfg_param_v1_t
{
    uint32_t cdc_reg_cfg_minor_version;
    uint32_t reg_logical_addr;
    uint32_t reg_field_type;
   uint32_t reg_field_bit_mask;
   uint16_t reg_bit_width;
   uint16_t reg_offset_scale;
}
;
typedef struct afe_cdc_reg_cfg_param_v2_t afe_cdc_reg_cfg_param_v2_t;
struct afe_cdc_reg_cfg_param_v2_t
{
    uint32_t minor_version;
    uint32_t reg_addr;
    uint32_t reg_field_type;
   uint32_t reg_field_bit_mask;
   uint16_t reg_bit_width;
   uint16_t reg_offset_scale;
}
;
typedef struct afe_cdc_reg_slimbus_cfg_param_v1_t afe_cdc_reg_slimbus_cfg_param_v1_t;
struct afe_cdc_reg_slimbus_cfg_param_v1_t
{
    uint32_t minor_version;
    uint32_t sb_la_offset;
}
;
typedef struct afe_cdc_aanc_version_param_v1_t afe_cdc_aanc_version_param_v1_t;
struct afe_cdc_aanc_version_param_v1_t
{
    uint32_t cdc_aanc_minor_version;
    uint32_t aanc_hw_version;
}
;
typedef struct afe_clip_bank_sel_cfg_v1_t afe_clip_bank_sel_cfg_v1_t;
struct afe_clip_bank_sel_cfg_v1_t
{
    uint32_t clip_bank_sel_cfg_minor_version;
    uint32_t num_banks;
    uint32_t bank_map[0x4];
}
;
typedef struct afe_cdc_reg_page_cfg_t afe_cdc_reg_page_cfg_t;
struct afe_cdc_reg_page_cfg_t
{
   uint32_t minor_version;
    uint32_t enable;
    uint32_t proc_id;
}
;
typedef struct afe_cdc_pwr_cntl_cfg_v1_t afe_cdc_pwr_cntl_cfg_v1_t;
struct afe_cdc_pwr_cntl_cfg_v1_t
{
  uint32_t minor_version;
  uint32_t enable;
}
;
typedef struct afe_param_id_lpaif_clk_cfg_t afe_param_id_lpaif_clk_cfg_t;
struct afe_param_id_lpaif_clk_cfg_t
{
    uint32_t lpaif_cfg_minor_version;
    uint32_t clk_value1;
    uint32_t clk_value2;
    uint16_t clk_src;
    uint16_t clk_root;
    uint16_t clk_set_mode;
    uint16_t reserved;
}
;
typedef struct afe_param_id_spdif_clk_cfg_t afe_param_id_spdif_clk_cfg_t;
struct afe_param_id_spdif_clk_cfg_t
{
    uint32_t spdif_clk_cfg_minor_version;
    uint32_t clk_value;
    uint32_t clk_root;
}
;
typedef struct afe_param_id_internal_digital_cdc_clk_cfg_t afe_param_id_internal_digital_cdc_clk_cfg_t;
struct afe_param_id_internal_digital_cdc_clk_cfg_t
{
    uint32_t cdc_clk_cfg_minor_version;
    uint32_t clk_value;
    uint16_t clk_root;
    uint16_t reserved;
}
;
typedef struct afe_port_custom_event_t afe_port_custom_event_t;
struct afe_port_custom_event_t
{
   uint32_t port_id;
   uint32_t module_id;
   uint32_t param_id;
   uint16_t event_type;
   uint16_t payload_size;
}
;
typedef struct afe_fbsp_feedback_path_cfg_v1_t afe_fbsp_feedback_path_cfg_v1_t;
struct afe_fbsp_feedback_path_cfg_v1_t
{
   uint32_t feedback_path_cfg_minor_version;
   int32_t dst_portid;
   int32_t num_channels;
   int32_t chan_info[4];
}
;
typedef struct afe_param_id_group_device_i2s_cfg_v1_t afe_param_id_group_device_i2s_cfg_v1_t;
struct afe_param_id_group_device_i2s_cfg_v1_t
{
    uint32_t minor_version;
    uint16_t group_id;
    uint16_t channel_mode;
    uint32_t sample_rate;
    uint16_t port_id[8];
    uint16_t bit_width;
    uint16_t reserved;
}
;
typedef struct afe_param_id_group_device_tdm_cfg_t afe_param_id_group_device_tdm_cfg_t;
struct afe_param_id_group_device_tdm_cfg_t
{
   uint32_t group_device_cfg_minor_version;
   uint16_t group_id;
   uint16_t reserved;
    uint16_t port_id[8];
    uint32_t num_channels;
    uint32_t sample_rate;
    uint32_t bit_width;
    uint16_t nslots_per_frame;
    uint16_t slot_width;
    uint32_t slot_mask;
}
;
typedef struct afe_param_id_group_device_enable_t afe_param_id_group_device_enable_t;
struct afe_param_id_group_device_enable_t
{
   uint16_t group_id;
   uint16_t enable;
}
;
typedef struct afe_cmd_add_topologies_t afe_cmd_add_topologies_t;
struct afe_cmd_add_topologies_t
{
    uint32_t data_payload_addr_lsw;
    uint32_t data_payload_addr_msw;
    uint32_t mem_map_handle;
    uint32_t buffer_size;
}
;
typedef struct afe_module_info_t afe_module_info_t;
struct afe_module_info_t
{
   uint32_t module_id;
   uint32_t use_lpm;
   uint8_t init_params[16];
}
;
typedef struct afe_topology_definition_t afe_topology_definition_t;
struct afe_topology_definition_t
{
   uint32_t topology_id;
   uint32_t num_modules;
   afe_module_info_t module_info[16];
}
;
typedef struct afe_custom_topologies_t afe_custom_topologies_t;
struct afe_custom_topologies_t
{
   uint32_t num_topologies;
}
;
typedef struct afe_topo_id_v1_t afe_topo_id_v1_t;
struct afe_topo_id_v1_t
{
   uint32_t topo_def_minor_version;
   uint32_t topology_id;
}
;
typedef struct afe_param_id_port_lat_mode_cfg_t afe_param_id_port_lat_mode_cfg_t;
struct afe_param_id_port_lat_mode_cfg_t
{
  uint32_t minor_version;
  uint32_t mode;
}
;
typedef struct afe_param_id_sw_duty_cycle_cfg_v1_t afe_param_id_sw_duty_cycle_cfg_v1_t;
struct afe_param_id_sw_duty_cycle_cfg_v1_t
{
    uint32_t minor_version;
    uint32_t sleep_time_ms;
}
;
typedef struct afe_param_id_group_device_hdmi_multistream_cfg_t afe_param_id_group_device_hdmi_multistream_cfg_t;
struct afe_param_id_group_device_hdmi_multistream_cfg_t
{
    uint32_t minor_version;
    uint16_t group_id;
    uint16_t port_id[8];
    uint16_t stream_id[8];
   uint16_t action_id;
}
;
typedef struct afe_cont_buf_latency_stats_v1_t afe_cont_buf_latency_stats_v1_t;
struct afe_cont_buf_latency_stats_v1_t
{
   uint32_t cont_buf_latency_stats_minor_version;
   uint32_t buf_time_stamp_lsw;
   uint32_t buf_time_stamp_msw;
   uint32_t rndr_time_stamp_lsw;
   uint32_t rndr_time_stamp_msw;
}
;
typedef struct afe_param_id_dev_timing_stats_t afe_param_id_dev_timing_stats_t;
struct afe_param_id_dev_timing_stats_t
{
    uint32_t minor_version;
    uint16_t device_direction;
    uint16_t reference_timer;
    uint32_t resync_flag;
    int32_t acc_drift_value;
    uint32_t ref_timer_abs_ts_lsw;
    uint32_t ref_timer_abs_ts_msw;
}
;
typedef struct afe_port_media_type_v1_t afe_port_media_type_v1_t;
struct afe_port_media_type_v1_t
{
    uint32_t minor_version;
    uint32_t sample_rate;
    uint16_t bit_width;
    uint16_t num_channels;
    uint16_t data_format;
    uint16_t reserved;
}
;
typedef struct afe_param_id_slot_mapping_cfg_t afe_param_id_slot_mapping_cfg_t;
struct afe_param_id_slot_mapping_cfg_t
{
    uint32_t minor_version;
    uint16_t num_channel;
    uint16_t bitwidth;
    uint32_t data_align_type;
    uint16_t offset[0x8];
}
;
typedef struct afe_param_id_custom_tdm_header_cfg_t afe_param_id_custom_tdm_header_cfg_t;
struct afe_param_id_custom_tdm_header_cfg_t
{
    uint32_t minor_version;
    uint16_t start_offset;
    uint16_t header_width;
    uint16_t header_type;
    uint16_t num_frame_repeat;
}
;
typedef struct afe_cmd_request_lpass_resources afe_cmd_request_lpass_resources;
struct afe_cmd_request_lpass_resources
{
   uint32_t resource_id;
};
;
typedef struct afe_cmd_request_lpass_dma_resources afe_cmd_request_lpass_dma_resources;
struct afe_cmd_request_lpass_dma_resources
{
   uint8_t dma_type;
   uint8_t num_read_dma_channels;
   uint8_t num_write_dma_channels;
   uint8_t reserved;
}
;
typedef struct afe_cmdrsp_request_lpass_resources afe_cmdrsp_request_lpass_resources;
struct afe_cmdrsp_request_lpass_resources
{
   uint32_t status;
   uint32_t resource_id;
}
;
typedef struct afe_cmdrsp_request_lpass_dma_resources afe_cmdrsp_request_lpass_dma_resources;
struct afe_cmdrsp_request_lpass_dma_resources
{
   uint8_t dma_type;
   uint8_t num_read_dma_channels;
   uint8_t num_write_dma_channels;
   uint8_t reserved;
}
;
typedef struct afe_cmd_release_lpass_resources afe_cmd_release_lpass_resources;
struct afe_cmd_release_lpass_resources
{
   uint32_t resource_id;
}
;
typedef struct afe_cmd_release_lpass_dma_resources afe_cmd_release_lpass_dma_resources;
struct afe_cmd_release_lpass_dma_resources
{
  uint8_t dma_type;
   uint8_t num_read_dma_channels;
   uint8_t num_write_dma_channels;
   uint8_t reserved;
}
;
typedef struct afe_port_cmd_mod_evt_cfg_t afe_port_cmd_mod_evt_cfg_t;
struct afe_port_cmd_mod_evt_cfg_t
{
 uint32_t minor_version;
 uint32_t port_id;
 uint32_t num_events;
}
;
;
typedef struct afe_port_cmd_mod_evt_cfg_payload_t
{
    uint32_t module_id;
    uint16_t instance_id;
    uint16_t reserved;
    uint32_t event_id;
    uint32_t reg_flag;
} afe_port_cmd_mod_evt_cfg_payload_t ;
;
typedef struct afe_port_mod_event_header_t
{
 uint32_t minor_version;
 uint32_t port_id;
 uint32_t module_id;
 uint16_t instance_id;
 uint16_t reserved;
 uint32_t event_id;
 uint32_t payload_size;
} afe_port_mod_event_header_t ;
;
HwdLpassPropertyType lpass_prop =
{
   0x07700000,
   GPIO_TYPE_UNSUPPORTED,
   IRQ_TYPE_UNSUPPORTED,
   0,
};
static uint32 pri_mi2s_ports[] =
{
    0x1000,
    0x101A,
    0x101C,
    0x101E,
    0x1001,
    0x101B,
    0x101D,
    0x101F
};
static uint32 sec_mi2s_ports[] =
{
    0x1002,
    0x1003
};
static lpasshwio_intf_port_info_t pri_mi2s_port_info =
{
    PRIMARY_MI2S,
    4,
    DATA_LINE_BIDIRECT,
    ( sizeof(pri_mi2s_ports)/sizeof(pri_mi2s_ports[0]) ),
    pri_mi2s_ports
};
static lpasshwio_intf_port_info_t sec_mi2s_port_info =
{
    SECONDARY_MI2S,
    4,
    DATA_LINE_BIDIRECT,
    ( sizeof(sec_mi2s_ports)/sizeof(sec_mi2s_ports[0]) ),
    sec_mi2s_ports
};
lpasshwio_prop_lpaif_struct_t lpaif_prop = {
    0x7708000,
    (0x1000*16),
    {0x07709000,
        0,
        2,
        {
            &pri_mi2s_port_info,
            &sec_mi2s_port_info
        },
        SUPPORTED},
        {0x07709040,
            2,
            {PRIMARY_PCM,SECONDARY_PCM, 0, 0},
            {4,4,0,0},
            {DATA_LINE_BIDIRECT,DATA_LINE_BIDIRECT, 0, 0},
            SUPPORTED},
            {0,
                0,
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                NOT_SUPPORTED},
                {0,
                    0,
                    NOT_SUPPORTED},
                    {0,
                        0,
                        {0,0,0,0},
                        NOT_SUPPORTED},
                        (0x300A0000),
};
lpasshwio_prop_afe_dma_struct_t audioif_dma_prop = {
    0x7708000,
    (0x1000*16),
    0x0770E000,
    0x07710400,
    0x07713000,
    0x07710428,
    0x07713028,
    0,
    0,
    2,
    2,
    46,
    1,
    (0x300A0000)
};
lpasshwio_prop_afe_dma_struct_t hdmiout_dma_prop = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
HwdAvtimerPropertyType avtimer_prop =
{
    0x7706000,
    0x1000*2,
    {
        {
            0x07706000,
            AVTIMER_HW_MODE_L_MN
        },
        {
            0,
            AVTIMER_HW_MODE_INVALID
        },
        {
            0,
            AVTIMER_HW_MODE_INVALID
        }
    },
    0x7707000,
    45,
    0x30040000
};
HwdResamplerPropertyType resampler_prop = {
    0,
    0,
    0,
    0
};
lpasshwio_vfr_prop_t vfr_prop = {
    0x7701000,
    0x1000,
    0x07701010,
    0x07701020,
    0x07701030,
    0x10000000,
    0x1,
    2,
    2,
    {59, 60},
    {{0x0, 0x1},
     {0x7, 0x1}},
    IRQ_TYPE_DEFAULT
};
lpasshwio_prop_riva_struct_t afe_riva_prop = {0, 0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_slimbus_struct_t afe_slimbus_prop =
{
    0,
    0,
    {
        {
            0,
            {0, 0}
        },
        {
            0,
            {0,0}
        }
    }
};
lpasshwio_prop_spdif_struct_t spdiftx_prop = {0, 0, 0, 0, 0, 0, 0};
lpasshwio_prop_hdmi_output_struct_t hdmi_output_prop = {
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0
};
lpm_prop_struct_t lpm_prop = {
    0x7718000,
    8*1024,
    8*1024,
    0,
    QURT_MEM_CACHE_WRITEBACK,
    2,
};
HwdGenericClkPropertyType genericclk_prop =
{
    0x7700000,
    (64*1024),
    (0x30050000),
    MMPM_CORE_ID_LPASS_AIF,
    7,
    {
        0x100,
        0x102,
        0x200,
        0x202,
        0x302,
        0x20000,
        0x20001,
    },
    2,
    {0x200, 0x202},
    {0x20000, 0x20001},
    {72282000, 72282000}
};
