#line 1 "/local/mnt/workspace/CRMBuilds/RPM.BF.2.1.3-00034-M9205AAAAANAZR-1_20191223_015705/b/rpm_proc/core/mproc/glink/xport_rpm/config/xport_rpm_devcfg.xml"
<!--










 









 
-->

<driver name="NULL">
  <device id="/dev/core/mproc/glink/xport_rpm">

    <props name="proc_info" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
      xport_rpm_devcfg_proc_info
    </props>

    <props name="remote_procs" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
      xport_rpm_devcfg_remote_procs
    </props>


    <props name="toc" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
      xport_rpm_devcfg_toc
    </props>


  </device>
</driver>
