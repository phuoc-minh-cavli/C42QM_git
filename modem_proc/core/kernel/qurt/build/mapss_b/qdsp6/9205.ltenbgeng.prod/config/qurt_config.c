/*=============================================================================

                                    cust_config_template.c

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
// S(adv,sec,base) sets the section attribute to (adv) for
//  CONFIG_ADVANCED_MEMORY builds, to (sec) for CONFIG_SECTIONS
//  builds, and to (base) for builds with neither flag

#if defined(CONFIG_ADVANCED_MEMORY)
#define S(adv,sec,base) __attribute__((section(adv)))
#else
#define S(adv,sec,base) __attribute__((section(base)))
#endif

#define SECTION(s, a)

#define CONFIG_DEFAULT                                            /* Expands to nothing, but can be a macro argument */
#ifdef GEN_CONFIG_HEADER
#define IDIM(x)
#define CONFIG_SEC_VAR(c,s,t,n,i)    extern typeof(t)       n
#define CONFIG_RW_VAR(c,t,n,i)       extern typeof(t)       n
#define CONFIG_RO_VAR(c,t,n,i)       extern typeof(t) const n
#define CONFIG_RW_SPACE(c,s,t,n,a,i) extern typeof(t)       n
#define CONFIG_ZI_SPACE(c,s,t,n,a)   extern typeof(t)       n
#define SET_CONFIG_VALUE(x,y)
#define DEFINE_MAPPING(v,xmlstr)
#define CONFIG_MAP_VAR(c,t,n,i)      extern typeof(t) const n
#define DEFINE_Q6CFG_MMAP(symbol, tlb_size_kib, mmap_tab_idx)
#else
#define IDIM(x) x
#define CONFIG_SEC_VAR(c,s,t,n,i)    typeof(t)       n   s = i    /* This (section config vars) will be deprecated soon */
#define CONFIG_RW_VAR(c,t,n,i)       typeof(t)       n     = i    /* This (R/W config vars) will be deprecated soon */
#define CONFIG_RO_VAR(c,t,n,i)       typeof(t) const n     = i    /* Preferred way to declare information variables */
#define CONFIG_RW_SPACE(c,s,t,n,a,i) typeof(t)       n a s = i    /* Declare an initialized space-providing variable */
#define CONFIG_ZI_SPACE(c,s,t,n,a)   typeof(t)       n a s        /* Declare an un-initialized space-providing variable */

#define EIP_BUILD_COPY               1
#define EIP_BUILD_XML                2
#define EIP_BUILD_MAP_RANGE          3
#define EIP_BUILD_MAP_VAR            4
#define EIP_BUILD_Q6CFG              5

/**
 * @brief Macro to setup EIP build record for a memory map that covers Q6 
 * configuration area. 'symbol' is the base virtual address of the memory 
 * map. Its value is patched by QuRT Image Builder
 */
#define DEFINE_Q6CFG_MMAP(symbol, tlb_size_kib, mmap_tab_idx) \
    extern unsigned int const symbol;                     \
    const struct tag_##symbol {                           \
        unsigned short len;                               \
        unsigned short type;                              \
        void const * addr;                                \
        unsigned short tlb_size;                          \
        unsigned short idx;                               \
    } eip_build_q6cfg_##symbol = {                        \
        sizeof( struct tag_##symbol ),                    \
        EIP_BUILD_Q6CFG,                                  \
        &symbol,                                          \
        tlb_size_kib,                                     \
        mmap_tab_idx                                      \
    }

#define SET_CONFIG_VALUE(x,y)                      \
   extern typeof(y) __attribute__((weak)) x;       \
   union __attribute__((packed)) padded_##x {      \
      typeof(x) value;                             \
      char pad[(sizeof(x)+3)&~3];                  \
   };                                              \
   __attribute__((section(".data.eip_build.build"))) \
   const struct tag_##x {                          \
      unsigned short len;                          \
      unsigned short type;                         \
      void *addr;                                  \
      unsigned copylen;                            \
      union padded_##x data;                       \
   } eip_build_##x = {                             \
      sizeof(struct tag_##x),                      \
      EIP_BUILD_COPY,                              \
      &x,                                          \
      sizeof(x),                                   \
      {y}                                          \
   };

#define DEFINE_MAPPING(v, xmlstr)                     \
   const struct tag_##v {                             \
      unsigned short len __attribute__((aligned(4))); \
      unsigned short type;                            \
      const void *addr;                               \
      unsigned rangelen;                              \
      char xml[sizeof(xmlstr)];                       \
   } eip_build_##v = {                                \
        sizeof(struct tag_##v),                       \
        EIP_BUILD_MAP_RANGE,                          \
        &v,                                           \
        sizeof(v),                                    \
        xmlstr                                        \
   };

#define DEFINE_MAPVAR(v)                              \
   const struct tag_##v {                             \
      unsigned short len __attribute__((aligned(4))); \
      unsigned short type;                            \
      const void *addr;                               \
      unsigned varlen;                                \
   } eip_build_##v = {                                \
        sizeof(struct tag_##v),                       \
        EIP_BUILD_MAP_VAR,                            \
        &v,                                           \
        sizeof(v)                                     \
   };

#define CONFIG_MAP_VAR(c,t,n,i)         typeof(t) const n = i; DEFINE_MAPVAR(n)

SET_CONFIG_VALUE(QURT_EIP_BUILD_PTR, (struct tag_QURT_EIP_BUILD_PTR *)0);
extern struct tag_QURT_EIP_BUILD_PTR QURT_EIP_BUILD[];
struct tag_QURT_EIP_BUILD_PTR *QURT_EIP_BUILD_PTR = QURT_EIP_BUILD;

const struct tag_sender {
   unsigned short len __attribute__((aligned(4)));
   unsigned short type;
   char data[170];
} eip_build_sender = {
   sizeof(struct tag_sender),
   EIP_BUILD_XML,
   "\246\000\000\000<?xml version=\"1.0\" encoding=\"utf-8\"?><build_vars>"
   "<effective_memsize value=\"18874368\"/><total_memsize value=\"18874"
   "368\"/><region_cache_policies value=\"\"/></build_vars>"
};

#include <string.h>

void QURTK_late_eip_build(void)
{
   struct tag_QURT_EIP_BUILD_PTR *p;

   p = QURT_EIP_BUILD_PTR;
   if (p) {
      while (p->len || p->type) {
         if (p->type == EIP_BUILD_COPY) {
                if(p->addr)
                memcpy(p->addr, &p->data, p->copylen);
         }
         p = (void *)((unsigned)p + p->len);
         }
      }
   }
#endif

#define CACHEALIGNED __attribute__((aligned(32)))
#define DIVUP(number,divisor) (((number)/(divisor))+(((number)%(divisor))>0))  /* Divide, rounding up */
#define ROUNDUP(number,divisor) (DIVUP((number),(divisor))*(divisor))          /* Round up to multiple */
#define ARRAY_INIT(size) [0 ... (size)-1]
#define ALIGNED_SPACE(size) typeof(struct { unsigned char a[size] __attribute__((aligned(size))); })   /* Suitable for simple declaration of aligned space */

#define THREAD_CONTEXT                             \
   typeof(struct {                                 \
         unsigned char a[320]            \
         __attribute__((aligned(32))); \
   })
#define KERNEL_PCB                                 \
   typeof(struct {                                 \
         unsigned char a[112]            \
         __attribute__((aligned(8))); \
   })
#define KERNEL_SPCB                                   \
   typeof(struct {                                    \
         unsigned char a[{:SIZEOF_SPCB}]              \
         __attribute__((aligned({:ALIGNOF_SPCB})));   \
   })
#define KERNEL_MAILBOX                                 \
   typeof(struct {                                     \
         unsigned char a[32]            \
         __attribute__((aligned(8))); \
   })

#if !defined (ISLAND_ONLY) || defined(GEN_CONFIG_HEADER)

CONFIG_RW_VAR("Priority for main thread",
              unsigned int,
              qurtos_main_priority,
              1);
CONFIG_RW_VAR("Bitmask for main thread",
              unsigned int,
              qurtos_main_bitmask,
              0xff);

#if 0X1000 != 0
#define ACTUALQURTOSSTACKSIZE (0X1000)
#elif defined(CONFIG_MP)
#define ACTUALQURTOSSTACKSIZE (0x7C0)
#else
#define ACTUALQURTOSSTACKSIZE (0x1C0)
#endif

CONFIG_SEC_VAR("Stack size for guest-OS stack associated with each user thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_stack_size,
              ROUNDUP(ACTUALQURTOSSTACKSIZE, 32));

#if 0X0 != 0
#define ACTUALQURTOSISLANDSTACKSIZE (0X0)
#else
#define ACTUALQURTOSISLANDSTACKSIZE (ACTUALQURTOSSTACKSIZE)
#endif

SET_CONFIG_VALUE(qurtos_island_stack_size, ROUNDUP(ACTUALQURTOSISLANDSTACKSIZE, 32));

struct objcache_config {
   short memory_blocks;
   short virt_regions;
   short phys_regions;
   short user_processes;
   short user_threads;
   short qurtos_threads;
   short shmem_regions;
};
#define OBJCACHE_DEFAULT (-1)
SET_CONFIG_VALUE(qurtos_objcache_config, ((struct objcache_config){.memory_blocks = OBJCACHE_DEFAULT, \
   .virt_regions = OBJCACHE_DEFAULT, \
   .phys_regions = OBJCACHE_DEFAULT, \
   .qurtos_threads = OBJCACHE_DEFAULT, \
   .shmem_regions = OBJCACHE_DEFAULT}));

CONFIG_SEC_VAR("Priority for reaper thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_reaper_priority,
              0x2F);
CONFIG_SEC_VAR("Tcb partition for reaper thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_reaper_tcb_partition,
              0);
CONFIG_SEC_VAR("Stack size for reaper thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_reaper_stack_size,
              4096);

CONFIG_RW_VAR("Qurt PIMUTEX priority",
              unsigned int,
              QURTK_max_pi_prio,
              75);

CONFIG_SEC_VAR("Set to non-zero to cause qurtos to have a separate virtual pool for shared memory",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              qurtos_split_virt_pool,
               0X0);
CONFIG_RO_VAR("Set autostack size total memory, this is the size of image pool", 
              unsigned int,
              qurtos_autostack_total_pages,
              0);

CONFIG_SEC_VAR("Set pager size total memory, this is the size of image pool",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_pager_total_pages,
              0X0);
CONFIG_ZI_SPACE("Stack space for main thread",
                __attribute__((section(".bss.qurtos"))),
                unsigned char [ROUNDUP(0x1000L,8)],
                qurtos_app_stack,
                CACHEALIGNED);

CONFIG_RW_VAR("Stack size for main thread",
              unsigned int,
              qurtos_main_stack_size,
              sizeof(qurtos_app_stack));

CONFIG_SEC_VAR("How many bytes at the beginning of each thread stack should be filled with F8F8F8F8 watermark?",
               __attribute__((section(".data.export"))),
              unsigned int,
              qurt_osam_stack_fill_count,
              0x80);

CONFIG_SEC_VAR("How many bytes should be filled with F8F8F8F8 watermark at each step to check stack usage?",
               __attribute__((section(".data.export"))),
              unsigned int,
              qurt_osam_stack_usage_fill_count,
             (ROUNDUP(0X10,4)));
CONFIG_RO_VAR("", 
              unsigned int,
              qurt_stid_reserve_num,
              0X0);

enum config_heap_type {
   QURTOS_CONFIG_HEAP_STATIC,   /* Statically allocated */
   QURTOS_CONFIG_HEAP_DYNAMIC,  /* Dynamically allocated */
   QURTOS_CONFIG_HEAP_ASLR,     /* Dynamically allocated at random address */
   QURTOS_CONFIG_HEAP_APPREGION /* Not allocated by QuRT -- left up to application */
};

struct config_heap_info {
   enum config_heap_type type;
   void *vaddr;
   unsigned size;
};
struct config_autostack_info {
    void *vaddr;
    unsigned size;
    unsigned lo_water_mark;
    unsigned hi_water_mark;
    unsigned const_stack_size;
};

#if 3 == 0
#if !defined(GEN_CONFIG_HEADER)
static unsigned char qurtos_app_heap[ROUNDUP(0x0L,4)] CACHEALIGNED __attribute__((section(".bss.qurtos")));
#endif
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_STATIC, qurtos_app_heap, sizeof(qurtos_app_heap) }
#elif 3 == 1
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_DYNAMIC, 0, ROUNDUP(0x0L,0x1000) }
#elif 3 == 2
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_ASLR, 0, ROUNDUP(0x0L,0x1000) }
#elif 3 == 3
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_APPREGION, 0, ROUNDUP(0x0L,0x1000) }
#endif

#if 0 != 0
unsigned int QURTK_autostack_array[0] CACHEALIGNED __attribute__((section(".bss.kernel")));
#define QURTKAUTOSTACKINIT { QURTK_autostack_array, sizeof(QURTK_autostack_array), 0, 0, 0 }
#else
#define QURTKAUTOSTACKINIT { 0, 0, 0, 0, 0 }
#endif

#ifdef CONFIG_ISLAND
SET_CONFIG_VALUE(qurtos_island_handle_count, 0);
#endif 
SET_CONFIG_VALUE(qurtos_qdi_handle_count, 1024);

CONFIG_SEC_VAR("Guest-OS application heap configuration",
              __attribute__((section(".data.export"))),
              struct config_heap_info,
              qurtos_app_heap_cfg,
              QURTOSAPPHEAPINIT);

CONFIG_RW_VAR("QURTK auto stack configuration",
              struct config_autostack_info,
              QURTK_autostack_array_cfg,
              QURTKAUTOSTACKINIT);

CONFIG_SEC_VAR("Hi watermark for autostack",
                __attribute__((section(".data"))),
                unsigned int,
                QURTK_hi_water_mark, 
                0);
                
CONFIG_SEC_VAR("Lo watermark for autostack",
                __attribute__((section(".data"))),
                unsigned int,
                QURTK_lo_water_mark,
                0);

CONFIG_RW_VAR("Timetest port number",
              unsigned int,
              QURTK_timetest_port,
              0x0);

CONFIG_SEC_VAR("The address of IMEM that contains the flag for initializing ISDB",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_isdb_flag_imem_addr,
               0x8600934L);

CONFIG_SEC_VAR("Flag to enable ETM trace to Q6ETB",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_to_q6etb,
               0);

#define Q6ETBDUMP {}
CONFIG_RW_VAR("Q6ETB dump",
              unsigned int [],
              qurt_q6etb_dump,
              Q6ETBDUMP);

CONFIG_RW_VAR("Interrupts that wake up the processor during TCXO shutdown; Most significant bit is INT 0",
              unsigned int,
              QURTK_tcxo_intmask,
              0xffffffffL);

CONFIG_SEC_VAR("flag to enable DMT scheduling",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_dmt_enabled,
               1);

CONFIG_SEC_VAR("flag to enable bus resources sharing",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_BQ_shared,
               0);

// QURTKTCMDUMPTYPE = 0: This is used in the error hndlr when "tcm" tag is not present
// in the config xml.
// QURTKTCMDUMPTYPE = 1: This is used in the error hndlr when "tcm" tag is present
#if 4 == 0
CONFIG_ZI_SPACE("Space for tcm tag is not defined in the config xml",
                __attribute__((section(".bss.qurtos"))),
                unsigned char [ROUNDUP(0X40000,32)],
                qurt_tcm_dump_static_loc,
                CACHEALIGNED);

#endif

CONFIG_ZI_SPACE("Space for TCM dump in the error handler",
                CONFIG_DEFAULT,
                unsigned char *,
                QURTK_tcm_err_dump,
                CONFIG_DEFAULT);

CONFIG_RW_VAR("Flag variable to know if TCM Dump is defined by user or QuRT",
              int,
              QURTK_tcm_dump_type,
              4);

// qurt_tcm_dump is a pointer. It will be pointing to qurt_tcm_dump_pool if "pool" is
// defined in tcm tag in the config xml and used for saving tcm going into airplane mode.
// If tcm_dump tag is not present, then it'll point to the qurt_tcm_dump_static_loc
CONFIG_SEC_VAR("Pointer to tcm save location during power collapse",
               __attribute__((section(".data"))),
               unsigned char *,
               qurt_tcm_dump,
               0);

CONFIG_SEC_VAR("Size for TCM dump",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURTK_tcm_dump_size,
              (ROUNDUP(0X40000,32)));

//this is the dynamically set size via modified qurt_power_control API
CONFIG_RW_VAR("Size for TCM save config, can be changed during execution",
              unsigned int,
              QURTK_tcm_save_size,
              0X0);

/****************************************************************************
 * Default SSR (Supervisor Status Register) settings                        *
 * Read Q6 Architecture system-level Specification before change            *
 * Incorrect setting of any bit will cause runtime system failure           *
 * bit 0 is LSB                                                             *
 * bit 16:  the privilege mode the SW thread will be running                *
 *          1->user mode (default); 0->supervisor mode                      *
 * bit 22:  BadVa selector.                                                 *
 *          1->BadVA1; 0->BadVA0 (default)                                  *
 * bit 23:  enable/disable reading PCYCLE register                          *
 *          1->enabled (default); 0->disabled                               *
 * bit 24:  enable/disable reading PMU                                      *
 *          1->enabled (default); 0->disabled                               *
 * bit 27-29:  XA coprocessor selection                                     *
 *          4->selection (default); 0->disabled                               *
 *                                                                          *
 *  Default SSR bits from MSB:  0010,0001,1000,0111, 0000,0000,0000,0000    *
 ****************************************************************************/

CONFIG_RW_VAR("Default SSR value",
              unsigned int,
              QURTK_ssr_default,
              0x21870000
#if defined(CONFIG_MP)
              | 0x00080000      /* Enable guest OS mode initially */
#endif //defined(CONFIG_MP)
              );
SET_CONFIG_VALUE(qurtos_ssr_default, 0x218F0000);


/****************************************************************************
 * Default CCR (Cache Control Register) settings                            *
 *                                                                          *
 *   bits 0-1, 3-4, 6-7 are cache CP bits, and set to 0 to use CCCC attr    *
 *                                                                          *
 *   bit 16:  HFI for L1 I cache hardware prefetch                          *
 *          1->honor the USR[HFI] settings(default); 0->disabled            *
 *   bit 17:  HFD for L1 D cache hardware prefetch                               *
 *          1->honor the USR[HFD] settings(default); 0->disabled            *
 *   bit 18:  HFI-L2 for L2 I cache hardware prefetch                       *
 *          1->honor the USR[HFIL2] settings(default); 0->disabled          *
 *   bit 19:  HFD-L2 for L2 D cache hardware prefetch                       *
 *          1->honor the USR[HFDL2] settings(default); 0->disabled          *
 *   bit 20:  SFD for  DCFECTCH/L2FETCH                                     *
 *          1->enalble DCFETCH/L2FETCH based on CCCC and partition(default) *
 *          0->disable DCFETCH and L2FETCH                                  *
 *                                                                          *
 *  Default CCR bits from MSB:  0000,0000,0001,0101, 0000,0000,0000,0000    *
 ****************************************************************************/

CONFIG_SEC_VAR("Default CCR value",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_config_ccr_default,
               0x150049);
SET_CONFIG_VALUE(qurtos_ccr_default, 0x150049);

/****************************************************************************
 * Default USR Cache Prefetch settings                                      *
 *                                                                          *
 *   bit 14-13: HFD.  Two bits for L1 data cache prefetching.               *
 *          00: No prefetching  (default)                                   *
 *          01: Prefetch Loads with post-updating address mode when         *
 *          execution is within a hardware loop.                            *
 *          Prefetch up to 4 cache lines.                                   *
 *          10: Prefetch any hardware-detected striding Load when           *
 *          execution is within a hardware loop.                            *
 *          Prefetch up to 4 cache lines.                                   *
 *          11: Prefetch any hardware-detected striding Load.               *
 *          Prefetch up to 4 cache lines.                                   *
 *   bit 16-15: HFI.  Two bits for L1 instruction cache prefetching.        *
 *          00: No prefetching                                              *
 *          01: prefetching of at most 1 additional cache line (default)    *
 *          10: prefetching of at most 2 additional cache lines             *
 *   bit 17:  This bit is reserved.                      *
 *   bit l9-18: HFI-L2. Two bits for L2 prefetching for instructions.       *
 *          00 = Disable L2 instruction prefetching for this thread.        *
 *          01 = Enable L2 instruction prefetching of 1 line.               *
 *          10 = Enable L2 instruction prefetching of 2 lines (default).    *
 *          11 = Enable L2 instruction prefetching of 4 lines.              *
 *                                                                          *
 *  Default USR bits from MSB:  0000,0000,0000,0101, 0000,0000,0000,0000    *
 ****************************************************************************/

CONFIG_SEC_VAR("Default USR value",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_config_usr_default,
               0x8000);
SET_CONFIG_VALUE(qurtos_usr_default, 0x8000);



/****************************************************************************
 * Configure L2 cache size                                                  *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The value in this variable doesn't have the exact size number       *
 *      in Kilobytes. Instead, this variable has the value that is          *
 *      assigned to register - SYSCFG:L2CFG. Please refer to Hexagon        *
 *      Architecture system-level specification for more information        *
 *      about system register SYSCFG and L2 cache size configuration.       *
 *      The valid L2 cache size is 0KB, 64KB, 128KB, 256KB, 512kB(V4M).     *
 *      The default size is set by Qurt to 128KB.                           *
 *      - 0 (sets 0KB L2 cache size)                                        *
 *      - 1 (sets 64KB L2 cache size)                                       *
 *      - 2 (sets 128KB L2 cache size)                                      *
 *      - 3 (sets 256KB L2 cache size)                                      *
 *      - 4 (sets 512KB L2 cache size)                                      *
 *      - 5 (sets 1024KB L2 cache size)                                      *
 *      - 100 (sets entire L2/TCM area as L2 cache)                         *
 ****************************************************************************/
#define kb_0   0
#define kb_64  1
#define kb_128 2
#define kb_256 3
#define kb_512 4
#define kb_1024 5
#define kb_full 100

CONFIG_SEC_VAR("L2 cache size configuration",
               __attribute__((section(".start"))),
               unsigned char,
               const QURTK_l2cache_size,
               kb_256);

CONFIG_RO_VAR("L1 Icache size configuration",
               unsigned int,
               QURTK_l1_icache_size,
               0X8000);
               
CONFIG_RO_VAR("L1 Dcache size configuration",
               unsigned int,
               QURTK_l1_dcache_size,
               0X4000);

/****************************************************************************
 * L2 cache non-write-allocate setting                                      *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L2 cache NWA is set to false by default if no input from XML.   *
 ****************************************************************************/

#define ENABLED     1
#define DISABLED    0
CONFIG_SEC_VAR("L2 cache non-write-allocate configuration",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_l2cache_nwa,
               DISABLED);


/****************************************************************************
 * L2 cache write-back setting                                              *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L2 cache write-back support is only available on V3+ target.    *
 *      By default, L2 cache is write-through.                              *
 ****************************************************************************/

#define ENABLED     1
#define DISABLED    0
CONFIG_SEC_VAR("L2 cache write-back configuration",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_l2cache_wb,
               ENABLED);


/****************************************************************************
 * I-cache enabling                                      *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The I-cache is enabled by default.                                  *
 *      This selection is only used for special needs (e.g. sw workaround). *
 ****************************************************************************/

#define ENABLED     1
#define DISABLED    0
CONFIG_SEC_VAR("I Cache Enabling",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_icache_enable,
               ENABLED);

/****************************************************************************
 * L1 cache size setting                                                    *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Enabling Half Icache or Dcache is only available on V3+ target.     *
 *      By default, Full I/D cache is used.                                 *
 ****************************************************************************/

CONFIG_SEC_VAR("L1 cache size setting",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_HI_cache,
               DISABLED);
CONFIG_SEC_VAR("L1 cache size setting",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_HD_cache,
               DISABLED);

/****************************************************************************
 * L1 cache partition settings                                              *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L1 data and instruction caches can be independently             *
 *      partitioned into two regions, a main partition and an auxiliary     *
 *      partition. The partitions are controlled by the ICPART and          *
 *      DCPART RW-bits in SYSCFG register. The feature is only              *
 *      available on v3+ targets.                                           *
 *                                                                          *
 *      The actual size of each cache partition also depends on the L1      *
 *      cache size setting. The following values are allowed for            *
 *      QURTK_DC_partition and QURTK_IC_partition:                        *
 *      - 0 (L1 cache is shared by all threads)                             *
 *      - 1 (L1 cache is split into two equal parts. Both main and          *
 *          auxiliary sections use 1/2 the cache)                           *
 *      - 2 (L1 cache is split into two parts. The main section is 3/4      *
 *          of the capacity, while the auxiliary section uses 1/4)          *
 *      - 3 (reserved)                                                      *
 ****************************************************************************/
#define full          0
#define half          1
#define three_quarter 2
#define resv          3

CONFIG_SEC_VAR("L1 data cache partition",
               __attribute__((section(".data"))),
               unsigned char,
               QURTK_DC_partition,
               full);

CONFIG_SEC_VAR("L1 instruction cache partition",
               __attribute__((section(".data"))),
               unsigned char,
               QURTK_IC_partition,
               full);

/****************************************************************************
 * Static (build-time) value for SYSCFG                                     *
 *                                                                          *
 * NOTES:                                                                   *
 *      Contains build-time configured bits/fields of SYSCFG as indicated   *
 *      by the mask. Only those bits/field indicated by mask are inserted   *
 *      into SYSCFG at boot in crt0.S:configure_adv_syscfg. The following   *
 *      variables are considered deprecated ...                             *
 *          - QURTK_BQ_shared                                               *
 *          - QURTK_dmt_enabled                                             *
 *          - QURTK_l2cache_nwa                                             *
 *          - QURTK_l2cache_wb                                              *
 *          - QURTK_l2cache_size                                            *
 *          - QURTK_DC_partition                                            *
 *          - QURTK_IC_partition                                            *
 *          - QURTK_L2_partition                                            *
 ****************************************************************************/
SET_CONFIG_VALUE(QURTK_syscfg_static, 0x838000);
SET_CONFIG_VALUE(QURTK_syscfg_static_mask, 0x7ea7a000);

/****************************************************************************
 * L2 Configuration Registers                                               *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Variables to hold a table of L2 configuration registers with        *
 *      initialization values provided during build-time (XML)              *
 ****************************************************************************/
#define L2CFG_TAB_LEN 0x0
CONFIG_RO_VAR( "Number of entries in L2 configuration register table",
               unsigned int,
               QURTK_l2cfg_tab_len,
               L2CFG_TAB_LEN );

#if L2CFG_TAB_LEN > 0

#define L2CFG_TAB {}

typedef struct l2cfg_tab_entry_s {
    unsigned int offset;
    unsigned int value;
} l2cfg_tab_entry_t;

CONFIG_RW_VAR( "L2 configuration register table",
               struct l2cfg_tab_entry_s [],
               QURTK_l2cfg_tab,
               L2CFG_TAB );
#endif

/****************************************************************************
 * L2 cache partition settings                                              *
 *                                                                          *
 * NOTES:                                                                   *
 *      L2 cache is partitioined into two regions, a main partition and     *
 *      an auxiliary. This are controlled by the L2PART in SYSCFG           *
 *      This feature is only available on v4+ targets.                      *
 *                                                                          *
 *      - 0 (L2 cache is fully shared)                                      *
 *      - 1 (L2 cache is split into two equal parts. Both main and          *
 *          auxiliary sections use 1/2 the cache)                           *
 *      - 2 (L2 cache is split into two parts. The main section is 3/4      *
 *          of the capacity, while the auxiliary section uses 1/4)          *
 *      - 3 (L2 cache is split into two parts. The main section is 7/8      *
 *          of the capacity, while the auxiliary section uses 1/8)          *
 ****************************************************************************/

// reuse the definitions on the above from 0 to 2, and add definition for 7/8
#define seven_eighths    3

CONFIG_SEC_VAR("L2 cache partition",
               __attribute__((section(".data"))),
               unsigned char,
               QURTK_L2_partition,
               full);


/****************************************************************************
 * Chicken bit settings (for HW bug workaround)                             *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_CHICKEN_BITS                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 ****************************************************************************/
/* Chicken bit settings */

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_rgdr,
               0x00000000);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_duck,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_turkey,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_chicken,
               0x08200004);

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_chicken_bits_rgdr_2nd,
               0);
CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_chicken_2nd,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_chicken_bits_rgdr_3rd,
               0);
CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_chicken_3rd,
               0);

#define SUPP_RGDR_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement rgdr bits",
              unsigned int [16],
              QURTK_supplement_rgdr,
              SUPP_RGDR_INIT);

#define SUPP_RGDRREV_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement rgdr revid",
              unsigned int [16],
              QURTK_supplement_rgdr_revid,
              SUPP_RGDRREV_INIT);

#define SUPP_TURKEY_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement turkey bits",
              unsigned int [16],
              QURTK_supplement_turkey,
              SUPP_TURKEY_INIT);

#define SUPP_TURKEYREV_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement turkey revid",
              unsigned int [16],
              QURTK_supplement_turkey_revid,
              SUPP_TURKEYREV_INIT);

#define SUPP_DUCK_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement duck bits",
              unsigned int [16],
              QURTK_supplement_duck,
              SUPP_DUCK_INIT);

#define SUPP_DUCKREV_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement duck revid",
              unsigned int [16],
              QURTK_supplement_duck_revid,
              SUPP_DUCKREV_INIT);

#define SUPP_CHICKEN_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement chicken bits",
              unsigned int [16],
              QURTK_supplement_chicken,
              SUPP_CHICKEN_INIT);

#define SUPP_CHICKENREV_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("Supplement chicken revid",
              unsigned int [16],
              QURTK_supplement_chicken_revid,
              SUPP_CHICKENREV_INIT);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_rgdr_num,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_turkey_num,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_duck_num,
               0);


CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_chicken_num,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_livelock_data,
               0x0);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_livelock_exception,
               0);

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_hvx_check_power_lock,
               1);

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_hvx_default_mode,
               2);


/****************************************************************************
 * Qurt Timer or Qtimer definitions                                        *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_QTIMERS                                                      *
 *                                                                          *
 * NOTES:                                                                   *
 *      Refer to your target HW reference manual for configuration          *
 *      of RGPT timer. Make sure the timer base address is one of           *
 *      the physical memory pools. Interrupt number does not conflict       *
 *      with RTOS reserved interrupts (check sched_config.h).               *
 ****************************************************************************/
CONFIG_SEC_VAR("",
               __attribute__((section(".data.export"))),
              unsigned int,
              QDSP6_QURT_TIMER_BASE,
              0xec120000L);
CONFIG_SEC_VAR("",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURT_timer_intno,
              2);
CONFIG_SEC_VAR("//254",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURT_timerIST_priority,
              64);
CONFIG_SEC_VAR("",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURT_timerIST_tcb_partition,
              0);


CONFIG_RO_VAR("",
              unsigned int,
              QURTK_dbg_cfg_mask,
              0X20000000);

CONFIG_RO_VAR("",
              unsigned int,
              QURTK_WB_dbg_cfg_mask,
              0X8000000);

#ifdef CONFIG_TIMED_FUTEX
SET_CONFIG_VALUE(QURTK_timer_freq_init,0x1);
#else
SET_CONFIG_VALUE(qurtos_timer_freq_init,0x1);
#endif

/****************************************************************************
 * Qurt Memory Pool Configuration                                          *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_DYNAMIC_MEMORY                                               *
 *                                                                          *
 * NOTES:                                                                   *
 *      QURT can have unlimited number of pools and each pool have a name  *
 *      and it can have up to 16 ranges. No gaps are allowed in ranges, e.g *
 *      if RANGE 2 is not defined in a pool the following range definitions,*
 *      e.g RANGE 3, RANGE 4 etc., are ignored                              *
 ****************************************************************************/
#if defined(CONFIG_DYNAMIC_MEMORY) || defined(GEN_CONFIG_HEADER)
struct phys_mem_pool_config{
    char name[32];
    struct range {
        unsigned int start;
        unsigned int size;
    } ranges[16];
};

#define POOLSTART(name) {name, {
#define POOLREGION(addr,size) {addr, size},
#define POOLEND {0}}},
#define POOL_CONFIGS_INIT { \
    POOLSTART("DEFAULT_PHYSPOOL") \
        POOLREGION(0x80000,0x1200000) \
    POOLEND \
    POOLSTART("HWIO_POOL") \
        POOLREGION(0x9000,0x1000000) \
    POOLEND \
    POOLSTART("ADSP_DRIVER_POOL") \
        POOLREGION(0x7700,0x20000) \
    POOLEND \
    POOLSTART("EBI1_pool") \
        POOLREGION(0x0,0x80000000) \
        POOLREGION(0x80000,0x80000000) \
    POOLEND \
    POOLSTART("TCM_POOL") \
        POOLREGION(0x4400,0x40000) \
    POOLEND \
    POOLSTART("CLADE_DICT") \
        POOLREGION(0x45D2,0x6000) \
    POOLEND \
    POOLSTART("") \
    POOLEND \
}
CONFIG_SEC_VAR("Memory pool config information",
               __attribute__((section(".data.qurtos"))),
              struct phys_mem_pool_config [],
              pool_configs,
              POOL_CONFIGS_INIT);
#endif /* QURT_DYNAMIC_MEMORY */

void qurtos_pool_image_fixups(void (*pfn)(const char *,void *,void *))
#ifdef GEN_CONFIG_HEADER
;
#else
{}


#endif

CONFIG_SEC_VAR("Number of entries to be locked in TLB",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_tlblock_entries,
               15);

CONFIG_RW_VAR(" Number of TLB entries to be reserved strictly for dynamic (page-table-based) memory mappings   "
              "   The default for this value is -1, which tells QuRT to synthesize a value based on the        "
              "   deprecated variable config_tlb_first_replaceable.                                            "
              " A minimum reasonable number for this value is 4 times the number of HW execution units         "
              "   on the processor; so 12 for v5, and 16 for v55 or v56.                                       "
              " This value may be set to zero, which disables page tables entirely.                            ",
              int,
              QURTK_dynamic_tlb_reserve,
              16);

CONFIG_RW_VAR("",
              unsigned int,
              config_tlb_first_replaceable,
              0);

CONFIG_RO_VAR("TLB debug level (0 for no debugging)",
              unsigned int,
              QURTK_tlb_debug_level,
              0X0);

#if 0X0 || defined(GEN_CONFIG_HEADER)

CONFIG_ZI_SPACE("TLB debug space",
                CONFIG_DEFAULT,
                ALIGNED_SPACE(32) [IDIM(4096)],
                QURTK_tlb_debug_space __attribute__((weak)),
                CONFIG_DEFAULT);

#endif

/* configure QURTK_min_image_page for image alignment */
CONFIG_SEC_VAR("Min_image_page for image alignment",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_min_image_page,
               0x40000);

/*
 * Configure static mappings array to add device memory mappings or shared
 * memory mapping
 *
 * vpn:         virtual page number
 * ppn:         physical page number
 * perm:        permission
 * cached_attr: cacheability
 * pgsize:      page size
 * others:      un-used
 *
 * Note: vpn and ppn has to be number of 4KB
 *
 * Note: please contact Qualcomm Technologies Inc. to verify the change.
 *       Incorrect change will crash the build in run time!
 */

#define R    1
#define W    2
#define RW   3
#define WR   3
#define X    4
#define RX   5
#define XR   5
#define WX   6
#define XW   6
#define RWX  7
#define XWR  7
#define KRN  0 /*< Accessible by kernel only */

#define MAIN  0
#define AUX   1

#define MEMORY_MAP(glob, asid, vpn, umode, busattr, perm, cache_attr, pgsize, cache_part, ppn) \
   ( ((unsigned long long)((1 << 31) | (glob << 30) | ((ppn & 0x800000) << 6) | (busattr << 27) | (asid << 20)|vpn) << 32 ) \
   | ( ((perm << 29) & 0xe0000000) | ((umode & 1) << 28) | (cache_attr << 24) | ((ppn & 0x7FFFFF) << 1) | pgsize))

#define MMAP_TABLE_INIT { \
   MEMORY_MAP(1, 0, 0x43c0L, 0, 1, RW, 0x4, 4, MAIN, 0x43c0L),\
   MEMORY_MAP(1, 0, 0x45d0L, 0, 1, RW, 0x4, 4, MAIN, 0x45d0L),\
   MEMORY_MAP(1, 0, 0xec000L, 1, 1, RW, 0x4, 16, MAIN, 0x4000L),\
   MEMORY_MAP(1, 0, 0xec100L, 1, 1, RW, 0x4, 16, MAIN, 0x4100L),\
   MEMORY_MAP(1, 0, 0xec200L, 1, 1, RW, 0x4, 16, MAIN, 0x4200L),\
   MEMORY_MAP(1, 0, 0xec300L, 1, 1, RW, 0x4, 16, MAIN, 0x4300L),\
   MEMORY_MAP(1, 0, 0xec5b0L, 1, 1, RW, 0x4, 4, MAIN, 0x45b0L),\
   MEMORY_MAP(1, 0, 0xec600L, 1, 1, RW, 0x1, 16, MAIN, 0x4600L),\
   MEMORY_MAP(1, 0, 0x4590L, 1, 0, RW, 0x6, 1, MAIN, 0x4590L),\
   MEMORY_MAP(1, 0, 0x45a0L, 1, 0, RW, 0x6, 1, MAIN, 0x45a0L),\
   MEMORY_MAP(1, 0, 0xe0ce0L, 1, 0, RW, 0x4, 4, MAIN, 0xe0L),\
   MEMORY_MAP(1, 0, 0xe1800L, 1, 0, RW, 0x4, 16, MAIN, 0x1800L),\
   MEMORY_MAP(1, 0, 0xbad10, 0, 0, KRN, 0x4, 4, MAIN, 0x800),\
   MEMORY_MAP(1, 0, 0xbad0c, 0, 0, KRN, 0x4, 1, MAIN, 0x600),\
   MEMORY_MAP(1, 0, 0xbad28, 0, 0, KRN, 0x4, 1, MAIN, 0x1400),\
   MEMORY_MAP(1, 0, 0x8628L, 0, 1, RW, 0x6, 1, MAIN, 0x8628L),\
   MEMORY_MAP(1, 0, 0x8600L, 0, 1, RW, 0x6, 4, MAIN, 0x8600L),\
   MEMORY_MAP(1, 0, 0x58L, 0, 1, RW, 0x4, 2, MAIN, 0x58L),\
   MEMORY_MAP(1, 0, 0x5cL, 0, 1, RW, 0x4, 2, MAIN, 0x5cL),\
   MEMORY_MAP(1, 0, 0xdcd00L, 0, 1, RW, 0x6, 1, MAIN, 0x81fbdL),\
   MEMORY_MAP(1, 0, 0xe002eL, 0, 0, RW, 0x4, 1, MAIN, 0x2eL),\
   MEMORY_MAP(1, 0, 0xe0131L, 0, 0, RW, 0x4, 1, MAIN, 0x31L),\
   MEMORY_MAP(1, 0, 0xe0132L, 0, 0, RW, 0x4, 1, MAIN, 0x32L),\
   MEMORY_MAP(1, 0, 0xe0233L, 0, 0, RW, 0x4, 1, MAIN, 0x33L),\
   MEMORY_MAP(1, 0, 0xe0350L, 0, 0, RW, 0x4, 2, MAIN, 0x50L),\
   MEMORY_MAP(1, 0, 0xe0460L, 0, 0, RW, 0x4, 1, MAIN, 0x60L),\
   MEMORY_MAP(1, 0, 0xe0461L, 0, 0, RW, 0x4, 1, MAIN, 0x61L),\
   MEMORY_MAP(1, 0, 0xe0462L, 0, 0, RW, 0x4, 1, MAIN, 0x62L),\
   MEMORY_MAP(1, 0, 0xe0588L, 0, 0, RW, 0x4, 1, MAIN, 0x88L),\
   MEMORY_MAP(1, 0, 0xe068eL, 0, 0, RW, 0x4, 1, MAIN, 0x8eL),\
   MEMORY_MAP(1, 0, 0xe0792L, 0, 0, RW, 0x4, 1, MAIN, 0x92L),\
   MEMORY_MAP(1, 0, 0xe0793L, 0, 0, RW, 0x4, 1, MAIN, 0x93L),\
   MEMORY_MAP(1, 0, 0xe0896L, 0, 0, RW, 0x4, 1, MAIN, 0x96L),\
   MEMORY_MAP(1, 0, 0xe09a0L, 0, 0, RW, 0x4, 4, MAIN, 0xa0L),\
   MEMORY_MAP(1, 0, 0xe0ab0L, 0, 0, RW, 0x4, 4, MAIN, 0xb0L),\
   MEMORY_MAP(1, 0, 0xe0bc0L, 0, 0, RW, 0x4, 4, MAIN, 0xc0L),\
   MEMORY_MAP(1, 0, 0xe0bd0L, 0, 0, RW, 0x4, 2, MAIN, 0xd0L),\
   MEMORY_MAP(1, 0, 0xe0d00L, 0, 0, RW, 0x4, 16, MAIN, 0x100L),\
   MEMORY_MAP(1, 0, 0xe0e00L, 0, 0, RW, 0x4, 16, MAIN, 0x200L),\
   MEMORY_MAP(1, 0, 0xe0f00L, 0, 0, RW, 0x4, 8, MAIN, 0x400L),\
   MEMORY_MAP(1, 0, 0xe0f40L, 0, 0, RW, 0x4, 8, MAIN, 0x440L),\
   MEMORY_MAP(1, 0, 0xe10a0L, 0, 0, RW, 0x4, 4, MAIN, 0x4a0L),\
   MEMORY_MAP(1, 0, 0xe11b0L, 0, 0, RW, 0x4, 2, MAIN, 0x4b0L),\
   MEMORY_MAP(1, 0, 0xe11b4L, 0, 0, RW, 0x4, 2, MAIN, 0x4b4L),\
   MEMORY_MAP(1, 0, 0xe1200L, 0, 0, RW, 0x4, 4, MAIN, 0x500L),\
   MEMORY_MAP(1, 0, 0xe1210L, 0, 0, RW, 0x4, 2, MAIN, 0x510L),\
   MEMORY_MAP(1, 0, 0xe1214L, 0, 0, RW, 0x4, 1, MAIN, 0x514L),\
   MEMORY_MAP(1, 0, 0xe1215L, 0, 0, RW, 0x4, 1, MAIN, 0x515L),\
   MEMORY_MAP(1, 0, 0xe1216L, 0, 0, RW, 0x4, 1, MAIN, 0x516L),\
   MEMORY_MAP(1, 0, 0xe1217L, 0, 0, RW, 0x4, 1, MAIN, 0x517L),\
   MEMORY_MAP(1, 0, 0xe1300L, 0, 0, RW, 0x4, 8, MAIN, 0x700L),\
   MEMORY_MAP(1, 0, 0xe1400L, 0, 0, RW, 0x4, 32, MAIN, 0x1000L),\
   MEMORY_MAP(1, 0, 0xe1900L, 0, 0, RW, 0x4, 16, MAIN, 0x1900L),\
   MEMORY_MAP(1, 0, 0xe1a00L, 0, 0, RW, 0x4, 16, MAIN, 0x2000L),\
   MEMORY_MAP(1, 0, 0xe1b00L, 0, 0, RW, 0x4, 16, MAIN, 0x2100L),\
   MEMORY_MAP(1, 0, 0xe1c00L, 0, 0, RW, 0x4, 16, MAIN, 0x2200L),\
   MEMORY_MAP(1, 0, 0xe1d00L, 0, 0, RW, 0x4, 16, MAIN, 0x2300L),\
   MEMORY_MAP(1, 0, 0xe1e00L, 0, 0, RW, 0x4, 16, MAIN, 0x2400L),\
   MEMORY_MAP(1, 0, 0xe1f00L, 0, 0, RW, 0x4, 16, MAIN, 0x2500L),\
   MEMORY_MAP(1, 0, 0xe2000L, 0, 0, RW, 0x4, 16, MAIN, 0x2600L),\
   MEMORY_MAP(1, 0, 0xe2100L, 0, 0, RW, 0x4, 16, MAIN, 0x2700L),\
   MEMORY_MAP(1, 0, 0xe2200L, 0, 0, RW, 0x4, 16, MAIN, 0x2800L),\
   MEMORY_MAP(1, 0, 0xe2300L, 0, 0, RW, 0x4, 16, MAIN, 0x2900L),\
   MEMORY_MAP(1, 0, 0xe2400L, 0, 0, RW, 0x4, 16, MAIN, 0x2a00L),\
   MEMORY_MAP(1, 0, 0xe2500L, 0, 0, RW, 0x4, 16, MAIN, 0x2b00L),\
   MEMORY_MAP(1, 0, 0xe2600L, 0, 0, RW, 0x4, 16, MAIN, 0x2c00L),\
   MEMORY_MAP(1, 0, 0xe2700L, 0, 0, RW, 0x4, 16, MAIN, 0x2d00L),\
   MEMORY_MAP(1, 0, 0xe2800L, 0, 0, RW, 0x4, 16, MAIN, 0x2e00L),\
   MEMORY_MAP(1, 0, 0xe2900L, 0, 0, RW, 0x4, 16, MAIN, 0x2f00L),\
   MEMORY_MAP(1, 0, 0xe2a00L, 0, 0, RW, 0x4, 16, MAIN, 0x3000L),\
   MEMORY_MAP(1, 0, 0xe2b00L, 0, 0, RW, 0x4, 16, MAIN, 0x3100L),\
   MEMORY_MAP(1, 0, 0xe2c00L, 0, 0, RW, 0x4, 16, MAIN, 0x3200L),\
   MEMORY_MAP(1, 0, 0xe2d00L, 0, 0, RW, 0x4, 16, MAIN, 0x3300L),\
   MEMORY_MAP(1, 0, 0xe2e00L, 0, 0, RW, 0x4, 16, MAIN, 0x3400L),\
   MEMORY_MAP(1, 0, 0xe2f00L, 0, 0, RW, 0x4, 16, MAIN, 0x3500L),\
   MEMORY_MAP(1, 0, 0xe3000L, 0, 0, RW, 0x4, 16, MAIN, 0x3600L),\
   MEMORY_MAP(1, 0, 0xe3100L, 0, 0, RW, 0x4, 16, MAIN, 0x3700L),\
   MEMORY_MAP(1, 0, 0xe3200L, 0, 0, RW, 0x4, 16, MAIN, 0x3800L),\
   MEMORY_MAP(1, 0, 0xe3300L, 0, 0, RW, 0x4, 16, MAIN, 0x3900L),\
   MEMORY_MAP(1, 0, 0xe3400L, 0, 0, RW, 0x4, 16, MAIN, 0x3a00L),\
   MEMORY_MAP(1, 0, 0xe3500L, 0, 0, RW, 0x4, 16, MAIN, 0x3b00L),\
   MEMORY_MAP(1, 0, 0xe3a00L, 0, 0, RW, 0x4, 16, MAIN, 0x6000L),\
   MEMORY_MAP(1, 0, 0xe3b00L, 0, 0, RW, 0x4, 16, MAIN, 0x6100L),\
   MEMORY_MAP(1, 0, 0xe3c00L, 0, 0, RW, 0x4, 16, MAIN, 0x6200L),\
   MEMORY_MAP(1, 0, 0xe3d00L, 0, 0, RW, 0x4, 16, MAIN, 0x6300L),\
   MEMORY_MAP(1, 0, 0xe3e00L, 0, 0, RW, 0x4, 16, MAIN, 0x6400L),\
   MEMORY_MAP(1, 0, 0xe3f00L, 0, 0, RW, 0x4, 16, MAIN, 0x6500L),\
   MEMORY_MAP(1, 0, 0xe4000L, 0, 0, RW, 0x4, 16, MAIN, 0x6600L),\
   MEMORY_MAP(1, 0, 0xe4100L, 0, 0, RW, 0x4, 16, MAIN, 0x6700L),\
   MEMORY_MAP(1, 0, 0xe4200L, 0, 0, RW, 0x4, 16, MAIN, 0x6800L),\
   MEMORY_MAP(1, 0, 0xe4300L, 0, 0, RW, 0x4, 16, MAIN, 0x6900L),\
   MEMORY_MAP(1, 0, 0xe4400L, 0, 0, RW, 0x4, 16, MAIN, 0x6a00L),\
   MEMORY_MAP(1, 0, 0xe4500L, 0, 0, RW, 0x4, 16, MAIN, 0x6b00L),\
   MEMORY_MAP(1, 0, 0xe4600L, 0, 0, RW, 0x4, 16, MAIN, 0x6c00L),\
   MEMORY_MAP(1, 0, 0xe4700L, 0, 0, RW, 0x4, 16, MAIN, 0x6d00L),\
   MEMORY_MAP(1, 0, 0xe4800L, 0, 0, RW, 0x4, 16, MAIN, 0x6e00L),\
   MEMORY_MAP(1, 0, 0xe4900L, 0, 0, RW, 0x4, 16, MAIN, 0x6f00L),\
   MEMORY_MAP(1, 0, 0xe4a00L, 0, 0, RW, 0x4, 4, MAIN, 0x7700L),\
   MEMORY_MAP(1, 0, 0xe4a10L, 0, 0, RW, 0x4, 4, MAIN, 0x7710L),\
   MEMORY_MAP(1, 0, 0xe4b80L, 0, 0, RW, 0x4, 8, MAIN, 0x7880L),\
   MEMORY_MAP(1, 0, 0xe4c00L, 0, 0, RW, 0x4, 8, MAIN, 0x7900L),\
   MEMORY_MAP(1, 0, 0xe4c40L, 0, 0, RW, 0x4, 4, MAIN, 0x7940L),\
   MEMORY_MAP(1, 0, 0xe4c50L, 0, 0, RW, 0x4, 4, MAIN, 0x7950L),\
   MEMORY_MAP(1, 0, 0xe4c60L, 0, 0, RW, 0x4, 4, MAIN, 0x7960L),\
   MEMORY_MAP(1, 0, 0xe4c70L, 0, 0, RW, 0x4, 1, MAIN, 0x7970L),\
   MEMORY_MAP(1, 0, 0xe4c71L, 0, 0, RW, 0x4, 1, MAIN, 0x7971L),\
   MEMORY_MAP(1, 0, 0xe4d80L, 0, 0, RW, 0x4, 8, MAIN, 0x7980L), 0 }
CONFIG_SEC_VAR("Empty1 0x008000- 0x070000 size = 0x068000 (416k)",
               __attribute__((section(".data.qurtos"))),
              unsigned long long [],
              qurtos_mmap_table,
              MMAP_TABLE_INIT);

CONFIG_SEC_VAR("Pointer to first unlocked entry in mmap table",
               __attribute__((section(".data.qurtos"))),
              unsigned long long *,
              qurtos_mmap_unlocked,
               &qurtos_mmap_table[15]);

/*
 * This belongs to named memsections used for named mmaps 
 * These are special memsections
 */
#define NAMED_MMAP_INIT { 0}

typedef struct qurtos_named_mmap_info
{
  char name[26];
  unsigned int paddr; 
  unsigned int vaddr; 
  unsigned int size;
  int prot;
  int user_mode;
  int cacheability;
  unsigned map_count;
}qurtos_named_mmap_info_t;

CONFIG_SEC_VAR("Named memsections are stored in a table here",
               __attribute__((section(".data.qurtos"))),
               qurtos_named_mmap_info_t [],
               qurtos_named_mmap_table,
               NAMED_MMAP_INIT);

struct unlock_after_boot {
   unsigned vpageno;
   unsigned pagecnt;
};

#define UNLOCK_AFTER_BOOT_INIT {\
   {0XE0CE0,0X10},\
   {0XE1800,0X100},\
   {0XBAD10,0X10},\
   {0XBAD0C,0X1},\
   {0XBAD28,0X1},\
   {0}\
}

CONFIG_SEC_VAR("List of memsections to be unlocked after boot",
               __attribute__((section(".data.qurtos"))),
              struct unlock_after_boot [],
              qurtos_unlock_after_boot,
              UNLOCK_AFTER_BOOT_INIT);

#ifndef GEN_CONFIG_HEADER

#endif

struct boot_mapping {
   unsigned long long template;
   char *start_ptr;
   char *end_ptr;
};

#define BOOT_MAPPING_INIT {{0}}
CONFIG_SEC_VAR("Symbol-based boot mappings",
               __attribute__((section(".data.qurtos"))),
              struct boot_mapping [],
              qurtos_boot_mappings,
              BOOT_MAPPING_INIT);

#define VIRT_RESERVE_INIT { 0x04000, 0x04FFF, 0xD0000, 0xD03FF, 0xDCC00, 0xDCCBF,  -1 }
CONFIG_SEC_VAR("Reserved virtual address ranges",
               __attribute__((section(".data.qurtos"))),
              int [],
              qurtos_virt_reserved,
              VIRT_RESERVE_INIT);

CONFIG_SEC_VAR("L2 cache partition data",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              QURTK_l2cache_partition_cfg,
              0x55555555);

CONFIG_SEC_VAR("Internal Bus Priority data",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              QURTK_bus_prio_cfg,
              0x0);

/****************************************************************************
 * Qurt L1 interrupt Configuration                                          *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Use QURTK_IEL to configure interrupt is                             *
 *      - level triggered (bit value = 0)                                   *
 *      - edge triggered (bit value = 1)                                    *
 *      Use QURTK_IAHL to configure interrupt is                            *
 *      - Active high (or rising edge) (bit value = 1)                      *
 *      - Active low (or falling edge) (bit value = 0)                      *
 *      interrupt 0 is at MSB.                                              *
 ****************************************************************************/

CONFIG_RW_VAR("0xffffffff",
              unsigned int,
              QURTK_IEL,
              0xffffffff);

/* 1 - Active High, 0 - Active Low. All bits are 1. Rising edge or active high */
CONFIG_RW_VAR("0xffffffff",
              unsigned int,
              QURTK_IAHL,
              0xffffffff);

CONFIG_RW_VAR("Bitmask of L1 interrupts allowed for nesting",
              unsigned int,
              QURTK_int_nest_allow,
              0X8000);

/****************************************************************************
 * Qurt L2 interrupt Configuration                                         *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_L2_INTERRUPT_ENABLED                                         *
 *                                                                          *
 * NOTES:                                                                   *
 *      Qurt kernel currently support up to 1024 L2VIC interrupts           *
 ****************************************************************************/

/* QURTK_l2_type[N] defines second level interrupt types, in which each bit
 * sets up the type of a L2 interrupt
 */
#define L2TYPE_INIT {0xf8ffe3e3,0x1fff91ff,0x8fb8225f,0xffffffff,0xffffffff,0xffffffff,0x1ff87c0,0xfb070e00,0xfffffff3,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff}
CONFIG_RW_VAR("interrupt 0 -> QURTK_l2_type[0]"
              "define L2 interrupt 0's type in LSB of QURTK_l2_type[N]"
              "0->level triggered; 1->edge triggered",
              unsigned int [32],
              QURTK_l2_type,
              L2TYPE_INIT);

#define ISR_INIT {}
CONFIG_RW_VAR("isr_int_no | isr_cb_no",
              unsigned short [],
              QURTK_isr_data,
              ISR_INIT);

/* Program Name length, set to the same value of QURT_PROCESS_ATTR_NAME_MAXLEN */
#define PROGRAM_NAME_LEN 64
#define PROGRAM_NAMES {"AMSS"}
CONFIG_SEC_VAR("Program names",
               __attribute__((section(".data.qurtos"))),
              char [][PROGRAM_NAME_LEN],
              QURTK_program_names,
              PROGRAM_NAMES);

/* This variable should eventually be filled in just from the XML;
   until we have a special tag in XML to designate the root process,
   we need to defer the computation of this variable.  Not until
   we run qurt-image-build do we actually have the information to
   figure out which slot corresponds to the root ELF.
   For now, if qurt-image-build.py sees this value as a negative
   number less than -1, it will attempt to fill the variable in
   with the information it has. */
CONFIG_SEC_VAR("Slot in QURTK_program_names which is a placeholder for the guest-OS process",
               __attribute__((section(".data.qurtos"))),
               int,
               QURTK_root_prog_no,
               -2);

CONFIG_SEC_VAR("",
               SECTION(".data", "QURTK.INTERRUPT"),
               unsigned int,
               QURTK_int_usrpd_check,
               1);

CONFIG_SEC_VAR("framekey usage",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURTK_stack_framekey_enabled,
              0);

/* Define stack memory for fast interrupt callback */

#ifndef CONFIG_LOW_FOOTPRINT

#ifndef GEN_CONFIG_HEADER
  #ifdef CONFIG_ISLAND_FASTINT
    unsigned long long qurtos_fastint_stack_island[4][DIVUP(1024,8)];
    SET_CONFIG_VALUE(qurtos_fastint_stack_size, sizeof(qurtos_fastint_stack_island[0]));
    SET_CONFIG_VALUE(qurtos_fastint_stack_addr, &qurtos_fastint_stack_island[0]);
  #else
    unsigned long long qurtos_fastint_stack_main[4][DIVUP(1024,8)];
    SET_CONFIG_VALUE(qurtos_fastint_stack_size, sizeof(qurtos_fastint_stack_main[0]));
    SET_CONFIG_VALUE(qurtos_fastint_stack_addr, &qurtos_fastint_stack_main[0]);
  #endif
#endif

#endif // CONFIG_LOW_FOOTPRINT

CONFIG_SEC_VAR("",
               SECTION(".data", "QURTK.INTERRUPT"),
               unsigned int,
               QURTK_int_max,
               259);
CONFIG_ZI_SPACE("",
                __attribute__((section(".bss.ukernel.QURTK_l2_interrupt_table.fast"))),
                ALIGNED_SPACE(16) [IDIM(259+1)],
                QURTK_l2_interrupt_table,
                CONFIG_DEFAULT);

CONFIG_RW_VAR("",
              unsigned int,
              QURTK_int_elem_size,
              sizeof(QURTK_l2_interrupt_table[0]));

CONFIG_ZI_SPACE("",
                __attribute__((section(".heap.qurtos"))),
                unsigned char [ROUNDUP(0X20000,4)],
                qurtos_heap,
                CACHEALIGNED);
SET_CONFIG_VALUE(QURTOS_HEAP_SIZE, sizeof(qurtos_heap));

#if 4695 || defined(GEN_CONFIG_HEADER)

CONFIG_RW_SPACE(
   "Pass-through XML directives to qurt-image-build.py",
   __attribute__((section(".data.qurtos_xml_passthrough.build"))),
   char [IDIM(4695)],
   qurtos_xml_passthrough __attribute__((weak)),
   CONFIG_DEFAULT,
   "S\022\000\000<image><program_name_map name=\"AMSS\" number=\"0\"/><bui"
   "ld>\n"
   "  <max_elf_tlb value=\"0x1000000\"/>\n"
   "  <relocatable_image value=\"false\"/>\n"
   "  <pool_allocation_order value=\"address\"/>\n"
   "  <tlbdump_usage value=\"compare\"/>\n"
   "  <mapping_method value=\"optimal\"/>\n"
   "  <allow_map_wx value=\"false\"/>\n"
   "  <section user_mode=\"true\"/>\n"
   "  <section name=\".start\" user_mode=\"false\"/>\n"
   "  <section name=\".init\" user_mode=\"false\"/>\n"
   "  <section mapping=\"rx\" name=\".data_tcm_static_kernel\"/>\n"
   "  <section mapping=\"rw\" name=\".data_tcm_static_fw\"/>\n"
   "  <!--<section name=\".data_tcm_static_sw\"     mapping=\"rw\"/>-->"
   "\n"
   "  <section mapping=\"overlay\" name=\"*_overlay\"/>\n"
   "  <section mapping=\"overlay\" name=\"overlay_*\"/>\n"
   "  <section mapping=\"overlay\" name=\".high_catm1_dynamic*\"/>\n"
   "  <section mapping=\"overlay\" name=\".low_catm1_dynamic*\"/>\n"
   "  <section cache_policy=\"1\" name=\"*_tcm_*\"/>\n"
   "  <section cache_policy=\"0\" name=\".data_l1wb_l2uc\"/>\n"
   "  <section cache_policy=\"6\" name=\".data_uncached\"/>\n"
   "  <section cache_policy=\"6\" name=\".bss_uncached\"/>\n"
   "  <section name=\"*_tcm_static_*\" physpool=\"TCM_POOL\"/>\n"
   "  <section mapping=\"rx\" name=\".keep_uncached\"/>\n"
   "\n"
   "  <section copybytes=\"4\" mapping=\"rx\" name=\".clade.dict\" phys"
   "pool=\"CLADE_DICT\" tlb_lock=\"boot\"/>\n"
   "  <section cache_policy=\"1\" mapping=\"rx\" name=\".clade.exceptio"
   "n_high\" physpool=\"TCM_POOL\"/>\n"
   "  <!-- used to create mapping between uncompression VA range and co"
   "mpression PA range -->\n"
   "  <section clade_register=\"clade_region_high_pd0\" name=\".region_"
   "high_clade_static\"/>\n"
   "  <section clade_register=\"clade_region_low_pd0\" name=\".region_l"
   "ow_clade_static\"/>\n"
   "  <!-- used to program CladeCompPDX register-->\n"
   "  <section clade_register=\"clade_comp\" mapping=\"rx\" name=\".cla"
   "de.comp\" tlb_lock=\"boot\"/>\n"
   "  <!-- used to prgram CladeExcHiPDX register-->\n"
   "  <section clade_register=\"clade_exception_high\" name=\".clade.ex"
   "ception_high\"/>\n"
   "  <!-- used to program CladeExcLowPDX register-->\n"
   "  <section clade_register=\"clade_exception_low_large\" mapping=\"r"
   "x\" name=\".clade.exception_low_large\" tlb_lock=\"boot\"/>\n"
   "  <!-- used to program CladeExcLowSmallPDX register-->\n"
   "  <section clade_register=\"clade_exception_low_small\" mapping=\"r"
   "x\" name=\".clade.exception_low_small\" tlb_lock=\"boot\"/>\n"
   "\n"
   "  <section mapping=\"rx\" name=\".clade.metadata\" tlb_lock=\"boot"
   "\"/>\n"
   "  <!-- used to program CladeRegion register -->\n"
   "  <clade_base_paddr value=\"0x20000000\"/>\n"
   "  \n"
   "  <!-- used to create mapping between uncompression VA range and co"
   "mpression PA range -->\n"
   "\011<section clade_register=\"clade2_region_pd0\" name=\".region_cl"
   "ade2\"/>\n"
   "\011 \n"
   "\011<section clade_register=\"clade2_comp\" mapping=\"rx\" name=\"."
   "clade2.comp\" tlb_lock=\"boot\"/>\n"
   "\011<section clade_register=\"clade2_free_base_addr_16\" mapping=\""
   "rx\" name=\".clade2.16B_free_list\" tlb_lock=\"boot\"/>\n"
   "\011<section clade_register=\"clade2_free_base_addr_32\" mapping=\""
   "rx\" name=\".clade2.32B_free_list\" tlb_lock=\"boot\"/>\n"
   "\011<section clade_register=\"clade2_free_base_addr_48\" mapping=\""
   "rx\" name=\".clade2.48B_free_list\" tlb_lock=\"boot\"/>\n"
   "\011<section clade_register=\"clade2_free_base_addr_64\" mapping=\""
   "rx\" name=\".clade2.64B_free_list\" tlb_lock=\"boot\"/>\n"
   "\011<section clade_register=\"clade2_metadata_base\" mapping=\"rx\""
   " name=\".clade2.metadata\" tlb_lock=\"boot\"/>\n"
   "\011<section clade_register=\"clade2_regs\" mapping=\"rx\" name=\"."
   "clade2.regs\" tlb_lock=\"boot\"/>\n"
   "\n"
   "  <!-- used to program Clade2 Region register -->\n"
   "        <clade2_base_paddr value=\"0xC0000000\"/>\n"
   "\n"
   "  <!--\n"
   "    override ELF segment RWX permissions; TODO: do this in ELF semg"
   "ents\n"
   "    instead of here\n"
   "  -->\n"
   "  <section mapping=\"rw\" name=\".hw_rodata\"/>\n"
   "  <section mapping=\"rw\" name=\".hw_rwdata\"/>\n"
   "  <section mapping=\"rw\" name=\".rodata.diag\"/>\n"
   "  <section mapping=\"rw\" name=\".rodata\"/>\n"
   "  <section mapping=\"rw\" name=\".data\"/>\n"
   "  <section mapping=\"rw\" name=\".9205_DEVCFG_DATA\"/>\n"
   "  <section mapping=\"rw\" name=\".eh_frame\"/>\n"
   "  <section mapping=\"rw\" name=\".gcc_except_table\"/>\n"
   "  <section mapping=\"rw\" name=\".ctors\"/>\n"
   "  <section mapping=\"rw\" name=\".dtors\"/>\n"
   "  <section mapping=\"rw\" name=\".bss\"/>\n"
   "  <section mapping=\"rw\" name=\".sdata\"/>\n"
   "  <section mapping=\"rx\" name=\"ro_fatal\"/>\n"
   "  <section mapping=\"rx\" name=\".candidate_compress_section\"/>\n"
   "  <section mapping=\"rx\" name=\".rw_candidate_compress_section\"/>"
   "\n"
   "  <section mapping=\"rw\" name=\".dynamicReclaim\"/>\n"
   "  <section name=\"*region_*_clade*\" tlb_lock=\"root\"/>\n"
   "  <section name=\"*region_*_clade*\" tlb_lock_min_size=\"0x100000\""
   "/>\n"
   "\n"
   "  <!--<section name=\"*\" tlb_lock=\"1\"/>-->\n"
   " </build><cache_policy name=\"writeback\" value=\"0x0\"/><cache_pol"
   "icy name=\"writethrough\" value=\"0x1\"/><cache_policy name=\"l1_wt"
   "_l2_cacheable\" value=\"0x5\"/><cache_policy name=\"l1_wb_l2_cachea"
   "ble\" value=\"0x7\"/><cache_policy name=\"l1_wb_l2_uncacheable\" va"
   "lue=\"0x0\"/><cache_policy name=\"l1_wb_l2_wt\" value=\"0x8\"/><cac"
   "he_policy name=\"uncached\" value=\"0x6\"/><cache_policy name=\"dev"
   "ice\" value=\"0x4\"/></image>"
);

#endif /* 4695 */

#ifdef CONFIG_ISLAND
SET_CONFIG_VALUE(qurtos_island_heap_size, ROUNDUP(0X0,4));
#endif
SET_CONFIG_VALUE(qurtos_max_user_processes, 0);
SET_CONFIG_VALUE(QURTK_MAX_USER_PROCESSES, 0);

CONFIG_ZI_SPACE("Array for space pointers in qurtos",
                __attribute__((section(".bss.qurtos"))),
                struct space * [IDIM(1+0)],
                g_spaces,
                CONFIG_DEFAULT);



/*
||  Some tricky manipulations here because we want a variable length
||   array of page table pointers which have a specific relationship
||   to some other variables in terms of layout in memory; however,
||   the debugger and OSAM functionality expects the page table
||   pointers to have a global symbol pointing at them.
||  So when we generate the actual cust_config object file, we
||   declare a structure and then use inline assembly to associate
||   global symbols to elements of the structure.
||  But when we generate the config header file, we just pretend
||   that the global symbols are normal C declarations.
*/

struct QURTK_VTLB_TABLE_DESCRIPTOR {
   unsigned vtlb_version:4;
   unsigned vtlb_table_type:3;
   unsigned next_table_addr_is_virtual:1;
   unsigned long long next_table_addr:36;
   unsigned table_entries:20;
};

struct QURTK_VTLB_BASIC_TABLE {
   // This structure is 160 bytes long and consists of fields used by OSAM/debugger
   //  4+4+8+8+128+8 == 160

   unsigned revision;                                              // Revision counter
   unsigned padding;                                               // Unused padding
   unsigned char signature[8];                                     // Table descriptor signature
   struct QURTK_VTLB_TABLE_DESCRIPTOR t0;                          // First table descriptor
   unsigned char asidmap[128];                                     // ASID map
   struct QURTK_VTLB_TABLE_DESCRIPTOR t1;                          // Second table descriptor
};

#ifdef CONFIG_VIRTUAL_TLB

#define VTLB_CLIENT_QURTOS 1

#ifdef CONFIG_SRM
#define VTLB_CLIENT_SRM 1
#else
#define VTLB_CLIENT_SRM 0
#endif

#ifdef CONFIG_SECURE_PROCESS
#define VTLB_CLIENT_CPZ 0X2
#else
#define VTLB_CLIENT_CPZ 0
#endif

#define VTLB_CLIENT_COUNT (VTLB_CLIENT_QURTOS+VTLB_CLIENT_SRM+VTLB_CLIENT_CPZ)

/*
||  VTLB_CLIENT_COUNT is the total number of VTLB clients.
||   Each VTLB client can only manipulate its own VTLB entries.
||   The guest-OS is one VTLB client.
||   Other possible VTLB clients include SRM (Secure Resource Manager),
||    and user processes which have migrated into CPZ.
||
||  The QURTK_vtlb_ownership[] bitmap contains a bit for each combination
||   of a VTLB entry index and a VTLB client index.  If a bit is '1', then
||   the given client and the given entry are associated with each other.
||   If a bit is '0', then they are not.
*/

SET_CONFIG_VALUE(QURTK_vtlb_client_bitmask, (1u << VTLB_CLIENT_COUNT) - 1);

CONFIG_ZI_SPACE("Ownership bitmask for VTLB entries",
                CONFIG_DEFAULT,
                unsigned [IDIM(DIVUP(2048*VTLB_CLIENT_COUNT,32))],
                QURTK_vtlb_ownership,
                CONFIG_DEFAULT);

#endif

#ifdef GEN_CONFIG_HEADER

extern unsigned QURTK_vtlb_ownership[];
extern unsigned short __attribute__((section(".data.ukernel.main"),aligned(32))) QURTK_page_table_v2[];
extern unsigned QURTK_tlb_available_count;
extern unsigned qurt_tlb_idx;
extern unsigned QURTK_tlb_first_replaceable;
extern unsigned QURTK_tlb_last_replaceable;
extern void *QURTK_pagetables[];
extern unsigned short QURTK_vtlb_tree[];
extern unsigned char QURTK_vtlb_asid_map[];
extern unsigned long long QURTK_vtlb_entries[];
extern unsigned QURTK_vtlb_extensions[];
extern unsigned QURTK_vtlb_entry_count;
extern unsigned QURTK_vtlb_tree_count;
extern unsigned QURTK_vtlb_prebiased_entry;
extern unsigned QURTK_vtlb_revision;
extern struct QURTK_VTLB_BASIC_TABLE QURTK_vtlb_basic_table;

#elif defined(CONFIG_VIRTUAL_TLB)

#define VTLB_TREE_COUNT 8192                               // Must be multiple of 4
#define VTLB_ENTRY_COUNT 2048

#if (VTLB_TREE_COUNT & 3)
#error VTLB_TREE_COUNT not a multiple of 4
#endif

struct tag_QURTK_VTLB_DATA {
   unsigned state[8] __attribute__((aligned(64)));                 // 8 words of state
   unsigned scratch[8];                                            // 8 words of scratch space
   unsigned short tree[VTLB_TREE_COUNT];                           // Lookup tree
   struct QURTK_VTLB_BASIC_TABLE descriptors;                      // OSAM/debugger descriptors
   unsigned long long entries[VTLB_ENTRY_COUNT];                   // VTLB entries
   unsigned extensions[VTLB_ENTRY_COUNT];                          // VTLB extensions
} QURTK_VTLB_DATA __attribute__((section(".data.ukernel.main"))) = {
   .state = {VTLB_ENTRY_COUNT, VTLB_TREE_COUNT},
   .descriptors = {.signature={'V','T','L','B',3,2,1,0}}
};

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

void *QURTK_pagetables[1] __attribute__((section(".data.ukernel.main"))) = { 0 };

__asm__(
    ".global QURTK_vtlb_entry_count\n"
    ".set    QURTK_vtlb_entry_count, QURTK_VTLB_DATA+0\n"
    ".size   QURTK_vtlb_entry_count, 4\n"
    ".global QURTK_vtlb_tree_count\n"
    ".set    QURTK_vtlb_tree_count, QURTK_VTLB_DATA+4\n"
    ".size   QURTK_vtlb_tree_count, 4\n"
    ".global QURTK_tlb_first_replaceable\n"
    ".set    QURTK_tlb_first_replaceable, QURTK_VTLB_DATA+16\n"
    ".size   QURTK_tlb_first_replaceable, 4\n"
    ".global QURTK_tlb_available_count\n"
    ".set    QURTK_tlb_available_count, QURTK_VTLB_DATA+20\n"
    ".size   QURTK_tlb_available_count, 4\n"
    ".global qurt_tlb_idx\n"
    ".set    qurt_tlb_idx, QURTK_VTLB_DATA+24\n"
    ".size   qurt_tlb_idx, 4\n"
    ".global QURTK_vtlb_prebiased_entry\n"
    ".set    QURTK_vtlb_prebiased_entry, QURTK_VTLB_DATA+28\n"
    ".size   QURTK_vtlb_prebiased_entry, 4\n"
    ".global QURTK_vtlb_tree\n"
    ".set    QURTK_vtlb_tree, QURTK_VTLB_DATA+64\n"
    ".size   QURTK_vtlb_tree, " STRINGIFY(VTLB_TREE_COUNT) "*2\n"
    ".global QURTK_vtlb_basic_table\n"
    ".set    QURTK_vtlb_basic_table, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2\n"
    ".size   QURTK_vtlb_basic_table, 160\n"
    ".global QURTK_vtlb_revision\n"
    ".set    QURTK_vtlb_revision, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2\n"
    ".size   QURTK_vtlb_revision, 4\n"
    ".global QURTK_vtlb_asid_map\n"
    ".set    QURTK_vtlb_asid_map, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2+24\n"
    ".size   QURTK_vtlb_asid_map, 4\n"
    ".global QURTK_vtlb_main\n"
    ".set    QURTK_vtlb_main, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2+16\n"
    ".size   QURTK_vtlb_main, 8\n"
    ".global QURTK_vtlb_entries\n"
    ".set    QURTK_vtlb_entries, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2+160\n"
    ".size   QURTK_vtlb_entries, " STRINGIFY(VTLB_ENTRY_COUNT) "*8\n"
    ".global QURTK_vtlb_extensions\n"
    ".set    QURTK_vtlb_extensions, QURTK_vtlb_entries+" STRINGIFY(VTLB_ENTRY_COUNT) "*8\n"
    ".size   QURTK_vtlb_extensions, " STRINGIFY(VTLB_ENTRY_COUNT) "*4\n"
);

#else

struct {
   unsigned long long _save[4];                                    // Offset 0, -64 from QURTK_page_table_v2
   unsigned pad[4];                                                // Offset 32, -32 from QURTK_page_table_v2
   unsigned _QURTK_tlb_available_count;                            // Offset 48, -16 from QURTK_page_table_v2
   unsigned _qurt_tlb_idx;                                         // Offset 52, -12 from QURTK_page_table_v2
   unsigned _QURTK_tlb_first_replaceable;                          // Offset 56, -8 from QURTK_page_table_v2
   unsigned _QURTK_tlb_last_replaceable;                           // Offset 60, -4 from QURTK_page_table_v2
   unsigned short _raw[20480];                            // Offset 64, -0 from QURTK_page_table_v2
} QURTK_pagetablestruct_v2 __attribute__((section(".data.ukernel.main"),aligned(32))) = {
   ._raw[1] = 640
};

void *QURTK_pagetables[1] __attribute__((section(".data.ukernel.main"))) = { 0 };

__asm__(
    ".global QURTK_TLBMISS_SCRATCH_AREA\n"
    ".set    QURTK_TLBMISS_SCRATCH_AREA, QURTK_pagetablestruct_v2+0\n"
    ".size   QURTK_TLBMISS_SCRATCH_AREA, 32\n"
    ".global QURTK_tlb_available_count\n"
    ".set    QURTK_tlb_available_count, QURTK_pagetablestruct_v2+48\n"
    ".size   QURTK_tlb_available_count, 4\n"
    ".global qurt_tlb_idx\n"
    ".set    qurt_tlb_idx, QURTK_pagetablestruct_v2+52\n"
    ".size   qurt_tlb_idx, 4\n"
    ".global QURTK_tlb_first_replaceable\n"
    ".set    QURTK_tlb_first_replaceable, QURTK_pagetablestruct_v2+56\n"
    ".size   QURTK_tlb_first_replaceable, 4\n"
    ".global QURTK_tlb_last_replaceable\n"
    ".set    QURTK_tlb_last_replaceable, QURTK_pagetablestruct_v2+60\n"
    ".size   QURTK_tlb_last_replaceable, 4\n"
    ".global QURTK_page_table_v2\n"
    ".set    QURTK_page_table_v2, QURTK_pagetablestruct_v2+64\n"
    ".size   QURTK_page_table_v2, 20480*2\n"
    ".global QURTK_page_table_v2_0x15\n"
    ".set    QURTK_page_table_v2_0x15, QURTK_pagetablestruct_v2+64+0x15\n"
    );

#endif

#ifdef CONFIG_VIRTUAL_TLB

#define VTLB_NOTIFY_SIZE (VTLB_ENTRY_COUNT%32)? ((VTLB_ENTRY_COUNT/32)+1): VTLB_ENTRY_COUNT/32
#define BITS_IN_INT (sizeof(unsigned)*8)
CONFIG_ZI_SPACE("VTLB notifications to island",
                __attribute__((section(".data"))),
                unsigned [IDIM(VTLB_ENTRY_COUNT/BITS_IN_INT)],
                QURTK_vtlb_notify,
                CONFIG_DEFAULT);
				/* Threads */
CONFIG_RW_VAR("",
              unsigned ,
              QURTK_vtlb_server_status,
              0);
#endif /* CONFIG_VIRTUAL_TLB */

/* Threads */
CONFIG_RW_VAR("",
              int,
              QURTK_config_MAX_THREADS,
              300);

/* Regular TCB storage */
CONFIG_ZI_SPACE("Regular TCB storage",
                __attribute__((section(".bss.export.QURTK_thread_contexts.fast"))),
                THREAD_CONTEXT [IDIM(300 - 0)],
                QURTK_thread_contexts_array,
                CONFIG_DEFAULT);

#ifndef GEN_CONFIG_HEADER
CONFIG_RW_VAR("",
              typeof(QURTK_thread_contexts_array[0]) *,
                QURTK_thread_contexts,
              QURTK_thread_contexts_array);

#endif

/* Mailbox memory */

CONFIG_RO_VAR("max mailboxes",
              unsigned int,
              QURTK_max_mailboxes,
              0X8);

CONFIG_ZI_SPACE("Mailbox Memory",
                S(".data.ukernel.main",".data.QURTK.SCHEDULER",".bss"),
                KERNEL_MAILBOX [IDIM(0X8)],
                QURTK_mailbox_data,
                CONFIG_DEFAULT);

CONFIG_RO_VAR("mailbox bandwidth",
              unsigned long long,
              QURTK_mailbox_msg_cycles,
              10000000/0X64);

#define INITIAL_BALANCE 8000    // 64k
CONFIG_RO_VAR("mailbox bandwidth",
              unsigned long long,
              QURTK_mailbox_max_cycles,
              INITIAL_BALANCE*(10000000/0X64));

/* Debug Buffer*/
CONFIG_RW_VAR("",
              unsigned int,
              QURTK_config_debug_buffer_size,
              0X2000);

CONFIG_ZI_SPACE("Array for Debug Buffer",
                CONFIG_DEFAULT,
                unsigned char [IDIM(0X2000)],
                QURTK_debug_buf,
                CONFIG_DEFAULT);


/* Uncached content for the kernel */

#if !defined(GEN_CONFIG_HEADER)

union {
   struct {
      /* START uncached kernel content */

#if defined(CONFIG_TRACEBUFFER)
      struct {
         char space[0X4000] __attribute__((aligned(16)));
      } trace_bufs[4];
      #define TBB_INIT {                         \
         &QURTK_uncached_content.trace_bufs,     \
         &QURTK_uncached_content.trace_bufs + 1, \
      }
      #define TBSIZE sizeof(QURTK_uncached_content.trace_bufs[0])
#else
      #define TBB_INIT { 0,0 }
      #define TBSIZE 0
#endif

#ifdef CONFIG_SAVE_L1DCACHE
   unsigned int l1dd_buf[64][8][10];
   #define L1DD_INIT QURTK_uncached_content.l1dd_buf 
   #define L1DDSIZE sizeof(QURTK_uncached_content.l1dd_buf)
#else
   #define L1DD_INIT 0
   #define L1DDSIZE 0
#endif /* #ifdef CONFIG_SAVE_L1DCACHE */

#ifdef CONFIG_DEBUG_L2
   unsigned int l2dd_buf[2048][8];
   #define L2DD_INIT QURTK_uncached_content.l2dd_buf 
   #define L2DDSIZE sizeof(QURTK_uncached_content.l2dd_buf)
#else
   #define L2DD_INIT 0
   #define L2DDSIZE 0
#endif /* #ifdef CONFIG_DEBUG_L2 */

      /* END uncached kernel content */
      int :0;                                   /* If there is no uncached content, this should prevent
                                                   a compiler error for empty struct.  A zero-length
                                                   unnamed bitfield should suffice as "dummy content"
                                                   while not adding any size to the actual struct.
                                                   Note that if there is no uncached content, this
                                                   should create a 64-byte variable that gets stripped
                                                   out later in garbage-collection. */
   };
   int ucdummy __attribute__((aligned(64)));    /* This makes sure that QURTK_uncached_content has both
                                                   an address and a size that are multiples of 64.
                                                   This is because we don't want to mix uncached and
                                                   cached content in the same 64-byte block. */
} QURTK_uncached_content;

#endif /* GEN_CONFIG_HEADER */

DEFINE_MAPPING(QURTK_uncached_content, "<section cache_policy=\"6:uncached\"/>");
                                                /* After this, CONFIG_MAP_VAR() can be used
                                                   to define variables which point into the
                                                   uncached area, and which get their virtual
                                                   addresses updated to the alternate mapping. */

CONFIG_MAP_VAR("",
               void * [2],
               QURTK_tbb,
               TBB_INIT);

CONFIG_MAP_VAR("Uncached area for dumping L1 D$ contents",
               void *,
               QURTK_L1Dcache_dump,
               L1DD_INIT);

CONFIG_MAP_VAR("Uncached area for dumping L2$ tag contents",
               void *,
               QURTK_l2tcm_tag,
               L2DD_INIT);

#if defined(CONFIG_TRACEBUFFER) || defined(GEN_CONFIG_HEADER)
typedef struct
{
   unsigned int   tbuf_size __attribute__((aligned(8)));
   unsigned int   tbuf_mask;
}
QURTK_trace_mask_t;
#define TRACE_INIT(a,b) { .tbuf_size=(a), .tbuf_mask=(b) }

CONFIG_RW_VAR("",
              unsigned int,
              QURTK_trace_buffer_size,
              TBSIZE);
SET_CONFIG_VALUE(qurtos_trace_buffer_size, TBSIZE);

CONFIG_SEC_VAR("",
               SECTION(".data", "QURTK.SCHEDULER"),
               QURTK_trace_mask_t,
               QURTK_config_trace_mask,
               TRACE_INIT(TBSIZE, 0x5FF));

CONFIG_RO_VAR("Trace mask to be set after a fatal error, in STM",
              unsigned int,
              QURTK_err_stm_trace_mask,
              0x5FF);
#endif // CONFIG_TRACE_BUFFER

typedef enum {
   QURTOS_INIT_STAGE_ZERO,            // Reserved
   QURTOS_INIT_STAGE_GENERIC,         // Install method handlers on the generic handle
} qurtos_init_stage_t;

typedef void (*qurtos_init_funcptr)(qurtos_init_stage_t);
void qurtos_call_init_funcs(qurtos_init_stage_t stage);

void qurtos_process_reaper_init(unsigned, unsigned, void *, unsigned);

#if 1

/*
|| If the process reaper is enabled, we will
||  call qurtos_process_reaper_init() and pass it the
||  configuration parameters during qurtos
||  initialization.
*/

void qurtos_process_reaper_generic_init(qurtos_init_stage_t stage)
{
   static CONFIG_ZI_SPACE("Process reaper stack",
                          __attribute__((section(".bss.user.config"))),
                          unsigned long long [IDIM(DIVUP(4096,8))],
                          process_reaper_stack,
                          CONFIG_DEFAULT);

   if (stage == QURTOS_INIT_STAGE_GENERIC) {
      qurtos_process_reaper_init(0x2F,
                                 0,
                                 process_reaper_stack,
                                 sizeof(process_reaper_stack));
   }
}

#endif /* PROCESS_REAPER_ENABLE */

#ifndef GEN_CONFIG_HEADER
#define INIT_FUNC_PROTO(n) void n(qurtos_init_stage_t)
INIT_FUNC_PROTO(qurtos_process_reaper_generic_init);
#endif

#define INIT_FUNCS { qurtos_process_reaper_generic_init, 0 }

CONFIG_SEC_VAR("List of initialization functions to be called at boot time",
               __attribute__((section(".rodata.qurtos"))),
              qurtos_init_funcptr [],
               const qurtos_config_init_funcs,
              INIT_FUNCS);

#ifndef GEN_CONFIG_HEADER

#endif

struct ramfs_scan_info {
   unsigned ppn_start;
   unsigned ppn_end;
   unsigned ppn_step;
};

#define QURTKRAMFSINIT {0,0,0}

CONFIG_SEC_VAR("Info about scanning for RAMFS partitions",
               __attribute__((section(".data.ukernel.main"))),
               struct ramfs_scan_info,
               QURTK_ramfs_scan,
               QURTKRAMFSINIT);


#ifdef  CONFIG_MEMORY_ECC

/********************************************
 * 
 *    Memory ECC Configuration
 *
 ********************************************/

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l1i_enable,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l1i_interrupt,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l1i_threshold,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l1d_enable,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l1d_interrupt,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l1d_threshold,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l2_enable,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l2_interrupt,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_l2_threshold,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_vtcm_enable,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_vtcm_interrupt,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_vtcm_threshold,
               0);

CONFIG_SEC_VAR("", __attribute__((section(".data"))),
               unsigned int, const QURTK_ecc_sec_int_num,
               0);

#endif //  CONFIG_MEMORY_ECC


#endif // !ISLAND_ONLY == MAIN_ONLY


#if !defined(MAIN_ONLY) || defined(GEN_CONFIG_HEADER)

#if 0X0 > 0 || defined(GEN_CONFIG_HEADER)
CONFIG_RW_SPACE("",
                __attribute__((section(".data.island"))),
                unsigned char [],
                qurtos_island_heap,
                CACHEALIGNED,
                {ARRAY_INIT(ROUNDUP(0X0,4)) = 0xFF});
#endif
CONFIG_RW_VAR("",
              int,
              QURTK_CONTEXT_SIZE,
              320);

CONFIG_RW_VAR("",
              int,
              QURTK_MAX_THREADS_IN_TCM,
              0);
SET_CONFIG_VALUE(qurtos_max_threads_in_tcm, 0);

CONFIG_RW_VAR("Bit mask to start HW threads, 0x3f to start all HW threads",
              unsigned int,
              QURTK_hthread_startup,
              0xf);

CONFIG_ZI_SPACE("TCM TCB storage",
                __attribute__((section(".bss.export.QURTK_thread_contexts_tcm.tcm"))),
                THREAD_CONTEXT [IDIM(0)],
                QURTK_thread_contexts_tcm,
                CONFIG_DEFAULT);

CONFIG_ZI_SPACE("Each id has two words",
                SECTION(".data", "QURTK.CONTEXTS"),
                unsigned long long [IDIM(300)],
                QURTK_thread_objs_array,
                CONFIG_DEFAULT);

#ifndef GEN_CONFIG_HEADER
unsigned long long *QURTK_thread_objs = &QURTK_thread_objs_array[0];
#endif

/* Futexes */
CONFIG_RW_SPACE("Futex space",
                SECTION(".data", "QURTK.FUTEX"),
                ALIGNED_SPACE(16) [IDIM(300)],
                QURTK_futex_objs_array,
                CONFIG_DEFAULT,
                {{{0}}});

#ifndef GEN_CONFIG_HEADER
CONFIG_RW_VAR("",
              typeof(QURTK_futex_objs_array[0]) *,
              QURTK_futex_objs,
              QURTK_futex_objs_array);
#endif

CONFIG_RW_VAR("",
              unsigned int,
              qdsp6ss_base_addr,
              0xec100000L);
CONFIG_RW_VAR("",
              unsigned int,
              QURTK_isr_default_prio,
              0X3C);
CONFIG_RW_VAR("",
              unsigned int,
              QURTK_isr_max_shared_int,
              0X1E);
CONFIG_RW_VAR("The address to jump to in case of abnormal/SDI reset",
               unsigned int,
               QURTK_crash_reset_addr,
               0x0);
			   
#ifdef CONFIG_CLADE
CONFIG_RW_VAR("CLADE CFG BASE",
              unsigned int,
              QURTK_clade_cfg_base,
              0x45d0000L);
CONFIG_RW_VAR("Clade expcetion high size in word",
              unsigned int,
              QURTK_clade_exc_hi_word,
              0x400L);
CONFIG_RW_VAR("Clade dictonary size in word",
              unsigned int,
              QURTK_clade_dict_word,
              0x1800L);
CONFIG_RW_SPACE("Array to save clade expcetion high in cold boot",
                __attribute__((section(".data.qurtos"))),
                unsigned int [],
                QURTK_clade_exception_hi,
                CACHEALIGNED,
                {ARRAY_INIT(0x400L+1) = 0xFFFFFFFF});
CONFIG_RW_SPACE("Array to save clade dictonary in cold boot",
                __attribute__((section(".data.qurtos"))),
                unsigned int [],
                QURTK_clade_dict,
                CACHEALIGNED,
                {ARRAY_INIT(0x1800L+1) = 0xFFFFFFFF});

CONFIG_RW_VAR("CLADE2 CFG BASE",
              unsigned int,
              QURTK_clade2_cfg_base,
              0x43c0000L);
#endif

#define L2ISLAND_INIT {}
CONFIG_RW_VAR("island interrupts",
              unsigned short [],
              QURTK_int_island,
              L2ISLAND_INIT);

#ifdef CONFIG_ISLAND
SET_CONFIG_VALUE(QURTK_max_island_int, 0);
#endif

CONFIG_ZI_SPACE("",
                SECTION(".data", "QURTK.INTERRUPT"),
                ALIGNED_SPACE(4) [IDIM(259+1)],
                QURTK_l2_int_idx_table,
                CONFIG_DEFAULT);

CONFIG_ZI_SPACE("",
                __attribute__((section(".bss.ukernel.QURTK_l2_island_interrupt_table.island"))),
                ALIGNED_SPACE(16) [IDIM(0)],
                QURTK_l2_island_interrupt_table,
                CONFIG_DEFAULT);

CONFIG_SEC_VAR("The address of SSC_SSC_ATB_LOW_POWER_HANDSHAKE register",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_ssc_atb_lphandshake,
               0x0);

CONFIG_SEC_VAR("The value ETM ATID",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_atid,
               0x24);

CONFIG_SEC_VAR("The value ETM ATB size, only 32-bit and 64-bit sizes are supported",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_atb_size,
               64);

CONFIG_SEC_VAR("flag to let ETM remain enabled after an exception",
               __attribute__((section(".data"))),
              unsigned int,
               QURTK_etm_enable_on_error,
               0);

CONFIG_RW_VAR("flag to raise a nonfatal error for island tlbmiss",
               unsigned int,
               QURTK_island_fail_tlbmiss,
               0x1);

/* Interrupt Privilege Control */
CONFIG_SEC_VAR("disabled=0, enabled=1",
              SECTION(".data", "QURTK.INTERRUPT"),
              unsigned int,
              QURTK_int_privilege,
              0);
SET_CONFIG_VALUE(qurtos_int_privilege, (char)(0!=0));

/* Define secure SID info */
CONFIG_RO_VAR("",
              unsigned int,
              QURTK_secure_proc_sid,
              0XFFFFFFFF);

/* Kernel PCB memory */
CONFIG_ZI_SPACE("PCB Memory",
                S(".data.ukernel.main",".data.QURTK.SCHEDULER",".bss"),
                KERNEL_PCB [IDIM(1+0)],
                QURTK_pcb_data,
                CONFIG_DEFAULT);

CONFIG_SEC_VAR("Maximum number of programs",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              QURTK_max_programs,
              1);

#define L2ENABLE_INIT {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
CONFIG_RW_VAR("disable=0, enable=1",
              unsigned int [],
              QURTK_l2_enable,
              L2ENABLE_INIT);

#ifndef GEN_CONFIG_HEADER
__attribute__((section(".text.ukernel.island")))
unsigned QURTK_mprotect(unsigned val, unsigned lo, unsigned hi)
{
  return val;
}
   // Filled in by Mprotect class in kernel_xml
#endif

#endif // !MAIN_ONLY == ISLAND_ONLY

#if !defined (ISLAND_ONLY) || defined(GEN_CONFIG_HEADER)

/* This goes at the end because it could conceivably have references to
   anything above, and we don't want to pollute with forward references */

#define PHYS_RELOC_INIT { 0 }
CONFIG_RO_VAR("Words in memory that should be offset at boot time"
              "  by the physical page number of the base of the image",
              const void * [],
              QURTK_phys_relocs,
              PHYS_RELOC_INIT);

struct genreloc {
   void *ptr __attribute__((aligned(8)));
   int delta;
};

#define GEN_RELOC_INIT { {0} }
CONFIG_RO_VAR("Words in memory that should be offset at boot time"
              "  by a general offset -- for instance, we use this"
              "  to shrink the default physpool at boot",
              const struct genreloc [],
              QURTK_gen_relocs,
              GEN_RELOC_INIT);

#endif // !ISLAND_ONLY == MAIN_ONLY

/* Give the declaration of the SRM reject function */
#define SRM_REJECT_DECL int QURTK_srm_reject(unsigned pageno, unsigned pgcnt)
#if defined(GEN_CONFIG_HEADER)
SRM_REJECT_DECL;                                /* Prototype for the real SRM reject function */
#else
SRM_REJECT_DECL ;                    /* Implementation of the real SRM reject function */
#endif

#ifndef GEN_CONFIG_HEADER
DEFINE_Q6CFG_MMAP( QURTK_l2cfg_base_vaddr, 64, 12 );
DEFINE_Q6CFG_MMAP( QURTK_etm_cfg_base, 4, 13 );
DEFINE_Q6CFG_MMAP( QURTK_fast_l2vic_port, 4, 14 );

#endif
