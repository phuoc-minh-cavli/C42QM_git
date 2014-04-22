/*==================-===========================================================
  Copyright (c) 2009 - 2011
  Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/dal/framework/src/DALGlbCtxt.c#1 $

when       who     description
--------   ---     -------------------------------------------------------------
01/17/11   sc      Removed all FEATURE_SPINLOCK_NOT_USED flags
10/01/10   sc      Stub out spinlock for SPS/ARM7
09/07/10   sc      Fixed the compiler warning
03/12/09   VK      Changed DALLOCK prefix to DALGLBCTXT
03/07/08   VK      Initial revision

==============================================================================*/

#include "stringl/stringl.h"
#include "assert.h"
#include "smem.h"

#include "dalconfig.h"
#include "timer.h"
#include "msg_diag_service.h"

#ifdef FEATURE_BMPOS_QNK
#define FEATURE_AMSS_ARM 1
extern void rex_yield(void);
#define AMSS_YIELD rex_yield
#endif

#ifdef FEATURE_L4
#include "kernel.h"
#define FEATURE_AMSS_ARM 1
extern void rex_yield(void);
#define AMSS_YIELD rex_yield
#endif

#ifdef  FEATURE_WINCE
#include "windows.h"
#endif

#ifdef  FEATURE_BLAST
#include "blast.h"
#endif

#include "DALSysInt.h"
#include "DALFramework.h"
#include "DALGlbCtxt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"

#include "DDIChipInfo.h"
#include "DDIHWIO.h"

#include "msmhwiobase.h"
#include "DALFwkHwio.h"
#include "HALhwio.h"

// pages size and lock area size in page units
#define DALGLBCTXT_PAGE_SIZE                    0x1000
#define DALGLBCTXT_SMEM_AREA_SIZE               0x1000
#define DALGLBCTXT_CACHE_LINE_SIZE_BYTES        16
#define DALGLBCTXT_SLEEP_MS                     1
#define DALGLBCTXT_MAX_SLEEP_MS                 500
#define DALGLBCTXT_strlen(name) ((strlen(name)<DALGLBCTXT_GLB_HDR_NAME_LEN)?strlen(name):(DALGLBCTXT_GLB_HDR_NAME_LEN-1))

uint8  *HWMutexBase        = NULL;
static DALGLB_HEADER * g_shared_token_hdr = NULL;
static qurt_mutex_t hGlbCtxtSync;
static uint32 HWMutexId    = 0;
static int GlbCtxtIsInit   = 0;
static DALResult (*InterProcSpinLock)(void);
static void (*InterProcSpinUnLock)(void);

static inline
uint32 _AllocGlbCtxtMem(void)
{
  // Add page size so as to always align it to page start
  return (uint32) smem_alloc(SMEM_DAL_AREA,(DALGLBCTXT_SMEM_AREA_SIZE+DALGLBCTXT_PAGE_SIZE));
}

static
void _LockYield(int times)
{
  uint32 dwIdx = 0;
  if(times)
  {
    // timed wait
    timer_sleep(times,T_MSEC,FALSE);
  }
}
     
static
void _PopulateLockHdr(DALGLB_HEADER * lock_hdr, uint32 size, const char *name)
{
  //populate the global header
  lock_hdr->size = size;
  memset(lock_hdr->name,0,DALGLBCTXT_GLB_HDR_NAME_LEN);
  strlcpy(lock_hdr->name,name,DALGLBCTXT_GLB_HDR_NAME_LEN);
  lock_hdr->lock.lock = 0;
  lock_hdr->lock.version = 0;
}

static
DALGLB_HEADER * _FindOrAllocHeader(const char *name, uint32 size_req)
{
  DALGLB_HEADER *glb_hdr = g_shared_token_hdr;
  uint32 size_avail = DALGLBCTXT_SMEM_AREA_SIZE;

  if( NULL == glb_hdr)
    return NULL;
  //Assume smem addresses are virt == phy address

  while(glb_hdr->size && size_avail)
  {
    // Add page size so as to always align it to page start
    if(name && (0 == memscmp(glb_hdr->name,name,DALGLBCTXT_strlen(name))))
      return glb_hdr;

    size_avail -= glb_hdr->size;
    glb_hdr = (DALGLB_HEADER *)(((uint32) glb_hdr) + glb_hdr->size);
  }

  return ((name || (size_avail < size_req))?NULL:glb_hdr);
}

extern uint32 DALFW_LockedGetW(volatile uint32 *pTarget);
extern DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget, uint32 value);

static void InterProcSWSpinUnLock(void)
{
  //see if Smem dll has been loaded for this process
  //Load the Smem driver module, if not loaded
  g_shared_token_hdr->lock.lock = 0;
}

static DALResult InterProcSWSpinLock(void)
{
  // Add page size so as to always align it to page start
  // but a temporary workaround for a HW issue
  int waitTime = 0;
  while (1)
  {
    if (g_shared_token_hdr->lock.lock == 0)
    {
      g_shared_token_hdr->lock.lock = DALGLBCTXT_SPINLOCK_PID;
      return DAL_SUCCESS;
    }

    if (waitTime > DALGLBCTXT_MAX_SLEEP_MS)
    {
      // check for time-out
      return DAL_ERROR;
    }
    //Assume virt == physical

    waitTime += DALGLBCTXT_SLEEP_MS;
    _LockYield(DALGLBCTXT_SLEEP_MS);
  }
}
      //For Bootloader env assume smem physical == virt and NOTE no API to mark attributes as strongly ordered
static void InterProcHWSpinUnLock(void)
{
  //just return back virtual adddr
  InterProcSWSpinUnLock();

  /* Write '0' to the hardware mutex to unlock */
  HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, 0 );
}

//Assume addresses are virt == phy address
static DALResult InterProcHWSpinLock(void)
{
  int waitTime = 0;
  DALResult ret;

  /* Attempt to write PID */
  HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, DALGLBCTXT_SPINLOCK_PID );

  while (HWIO_INI( TCSR_MUTEX_REG_n, HWMutexId) != 
         DALGLBCTXT_SPINLOCK_PID )
  {
    if (waitTime > DALGLBCTXT_MAX_SLEEP_MS)
    {
      return DAL_ERROR;
    }
    waitTime += DALGLBCTXT_SLEEP_MS;
    _LockYield(DALGLBCTXT_SLEEP_MS);

    /* Attempt to write PID again */
    HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, DALGLBCTXT_SPINLOCK_PID );
  }

  //Assume virt == physical

  //assume smem support
  //just return back virtual adddr
  ret = InterProcSWSpinLock();
  if (DAL_SUCCESS != ret)
  {
    HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, 0 );
  }
  return ret;
}

static void InterProcSpinInit(void)
{
  DalDeviceHandle * hChipinfo;
  DalDeviceHandle * hHWIO;
  DalChipInfoFamilyType family;
  DalChipInfoVersionType version;
  DALResult ret;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  DALSYSPropertyVar glbCtxtProp;

  // determine if HW spin lock can be used based on chip info

  ret = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipinfo);
  ASSERT(ret == DAL_SUCCESS && hChipinfo != NULL);
  DalChipInfo_GetChipFamily(hChipinfo, &family);
  DalChipInfo_GetChipVersion(hChipinfo, &version);

  if ((DALCHIPINFO_FAMILY_MDM9x25 == family && version < 0x00020001) ||
      (DALCHIPINFO_FAMILY_MSM8974 == family && version < 0x00020000))
  {
    InterProcSpinLock = InterProcSWSpinLock;
    InterProcSpinUnLock = InterProcSWSpinUnLock;
    
    MSG_ERROR("InterProcSpinInit: using SW spin lock", 0, 0, 0);
    return;
  }
  //For Bootloader env assume smem physical == virt and NOTE no API to mark attributes as strongly ordered

  ret = DALSYS_GetDALPropertyHandle(0,hProp);
  ASSERT(ret == DAL_SUCCESS);
  if (DAL_SUCCESS != DALSYS_GetPropertyValue(hProp,"GlbCtxtHWMutexNumber",0,&glbCtxtProp))
  {
    //populate the global header
    InterProcSpinLock = InterProcSWSpinLock;
    InterProcSpinUnLock = InterProcSWSpinUnLock;
    MSG_ERROR("InterProcSpinInit: GlbCtxtHWMutexNumber property is not defined", 0, 0, 0);
    return;
  }

  HWMutexId = glbCtxtProp.Val.dwVal;
  InterProcSpinLock = InterProcHWSpinLock;
  InterProcSpinUnLock = InterProcHWSpinUnLock;


  ret = DAL_DeviceAttach(DALDEVICEID_HWIO, &hHWIO);
  ASSERT(ret == DAL_SUCCESS && hHWIO != NULL);
  ret = DalHWIO_MapRegion(hHWIO, HW_MUTEX_BASE_NAME, &HWMutexBase);
  ASSERT(ret == DAL_SUCCESS && HWMutexBase != NULL);
}

static DALResult GlbCtxtLock(void)
{
   DALResult ret;
   // initialize to the smem region start

   qurt_mutex_lock(&hGlbCtxtSync);

   // Get to next free location
   ret = InterProcSpinLock();
   if (DAL_SUCCESS != ret)
   {
      qurt_mutex_unlock(&hGlbCtxtSync);
   }
   return ret;
}
     // are we searching for specific name
static void GlbCtxtUnLock(void)
{

   InterProcSpinUnLock();
   // return null if search for existing ctxt, or if nothing
   //   available for new ctxt
   qurt_mutex_unlock(&hGlbCtxtSync);
}

DALResult
DALGLBCTXT_Init()
{
  DALResult ret          = DAL_ERROR;
  uint32 DALSmemBaseAddr = 0;
  DALGLB_HEADER *glb_hdr = 0;

  if(NULL != &hGlbCtxtSync)
  {
    qurt_mutex_init(&hGlbCtxtSync);
  }

  qurt_mutex_lock(&hGlbCtxtSync);

  //prevent other thread on the same core.
  if (GlbCtxtIsInit)
  {
    // no re-initialization if last run was successful
    ret = DAL_SUCCESS;
    goto exit_dalglbctxt_init;
  }

  DALSmemBaseAddr = _AllocGlbCtxtMem();
  glb_hdr = (DALGLB_HEADER *)DALSmemBaseAddr;

  // return DAL_ERROR if we cannot alloc
  if(0 == DALSmemBaseAddr)
    goto exit_dalglbctxt_init;

  // Round the next avail page boundary, if not on page boundary
  //set the size of the un-aligned location to point to page boundary
  if(DALSmemBaseAddr&(DALGLBCTXT_PAGE_SIZE-1))
    glb_hdr->size = ((DALSmemBaseAddr+DALGLBCTXT_PAGE_SIZE) & 
          (~(DALGLBCTXT_PAGE_SIZE-1))) - DALSmemBaseAddr;

  if(!g_shared_token_hdr)
  {
    g_shared_token_hdr = (DALGLB_HEADER *)((uint32)DALSmemBaseAddr+ glb_hdr->size);
  }

  //this var is on a per core basis.
  InterProcSpinInit();
  //okay to assign this field again, or simultaneously as this is harmless.

  if(DAL_SUCCESS == InterProcSpinLock())
  {
    if(0 != memscmp(g_shared_token_hdr->name,"dalspinlock",11))
    {
      // zero out memory
      memset((void *)g_shared_token_hdr,0,DALGLBCTXT_SMEM_AREA_SIZE);

      // populate and init the dal specific region
      _PopulateLockHdr(g_shared_token_hdr,sizeof(DALGLB_HEADER),"dalspinlock");
    }
    //release lock
    InterProcSpinUnLock();
    GlbCtxtIsInit = 1;
    ret = DAL_SUCCESS;
  }
  else
  {
    //error, so memory must be in non zero state....
    MSG_ERROR("Cannot lock in DALGLBCTXT_Init: lock:0x%x", g_shared_token_hdr->lock.lock, 0, 0);
  }

exit_dalglbctxt_init:
   qurt_mutex_unlock(&hGlbCtxtSync);
   return ret;
}

DALResult
DALGLBCTXT_AllocCtxt(const char * name, uint32 size_req, uint32 lock_type, void ** ctxt)
{
  // search for an existing header
  DALGLB_HEADER * _hdr = 0;
  DALResult ret        = DAL_ERROR;

  if (!GlbCtxtIsInit)
  {
    if (DAL_SUCCESS != DALGLBCTXT_Init())
    {
      // failed to initialize
      return DAL_ERROR;
    }
  }

  // round size_req to cache line size
  if(size_req&(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1))
  {
    size_req = (size_req+DALGLBCTXT_CACHE_LINE_SIZE_BYTES) & ~(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1);
  }

  //grab the global lock to prevent other cores from accessing.
  if(DAL_SUCCESS == GlbCtxtLock())
  {
    //check to see if this previously exists
    _hdr = _FindOrAllocHeader(name,0);

    //if none exists, then allocate mem area
    if(!_hdr)
    {
      _hdr = _FindOrAllocHeader(NULL,size_req);

      //cannot allocate return error
      if(NULL == _hdr)
        goto exit_dal_alloc_ctxt;

      // populate and init the dal specific region
      _PopulateLockHdr(_hdr,size_req,name);
    }

    if(ctxt)
      *ctxt = (void *)_hdr;

    ret = DAL_SUCCESS;

exit_dal_alloc_ctxt:
      //release this lock for other cores
      GlbCtxtUnLock();
  }
  else
  {
    MSG_ERROR("Cannot lock in DALGLBCTXT_AllocCtxt: lock:0x%x", g_shared_token_hdr->lock.lock, 0, 0);
  }

  return ret;
}

DALResult
DALGLBCTXT_FindCtxt(const char * name, void ** ctxt)
{
  // search for an existing header
  DALGLB_HEADER * _hdr = 0;

  if (!GlbCtxtIsInit)
  {
    if (DAL_SUCCESS != DALGLBCTXT_Init())
    {
      return DAL_ERROR;
    }
  }

  if(DAL_SUCCESS == GlbCtxtLock())
  {
    _hdr = _FindOrAllocHeader(name,0);

    if(ctxt)
      *ctxt = (void *)_hdr;

    GlbCtxtUnLock();
  }
  else
  {
    MSG_ERROR("Cannot lock in DALGLBCTXT_FindCtxt: lock:0x%x", g_shared_token_hdr->lock.lock, 0, 0);
  }
  
  return (_hdr?DAL_SUCCESS:DAL_ERROR);
}
