#line 1 "/local/mnt/workspace/CRMBuilds/RPM.BF.2.1.3-00034-M9205AAAAANAZR-1_20191223_015705/b/rpm_proc/core/bsp/rpm/src/rpm.scl"











 










RPMSS_CODE 0x000000
{
  CODE_RAM 0x000000 0x1FFFF
  {
    startup.o(RPM_ENTRY, +FIRST)
    * (InRoot$$Sections)
    * (+RO-CODE)
  }
  DDR_CODE_RAM +0x0 FIXED NOCOMPRESS 
  {
    *rpm_proc/core/boot/ddr*.lib (+RO-CODE)
  }
  MPROC_CODE_RAM +0x0 FIXED NOCOMPRESS
  {
    *rpm_proc/core/mproc/glink/xport*.lib (+RO-CODE)
	*rpm_proc/core/mproc/smem*.lib (+RO-CODE)
	*rpm_proc/core/mproc/glink*.lib (+RO-CODE)
  }
}

RPMSS_DATA 0x90000 0x10000
{
  DATA_RAM 0x90000 NOCOMPRESS ZEROPAD
  {
    * (+RW)
    * (+ZI)
  }
  DATA_RAM_RO_DATA +0x0 FIXED NOCOMPRESS
  {
    * (+RO-DATA)
  }

  OTHER_SECTIONS +0x0 EMPTY 0x0
  {
    
  }

  INIT_DATA_RAM_STACK_END (0x90000 + 0x10000) EMPTY 0x0
  {
  }
}

