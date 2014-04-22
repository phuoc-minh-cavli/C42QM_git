#line 1 "/local/mnt/workspace/CRMBuilds/RPM.BF.2.1.3-00034-M9205AAAAANAZR-1_20191223_015705/b/rpm_proc/core/buses/spmi/src/platform/config/bear/9x05/spmi_rpm.xml"
<!--
 * @brief: Configuration file for the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime:$
 * $Header:$
 * $Change:$
-->
<driver name="NULL">
    <device id="DALDEVICEID_SPMI_DEVICE">

#line 1 "/local/mnt/workspace/CRMBuilds/RPM.BF.2.1.3-00034-M9205AAAAANAZR-1_20191223_015705/b/rpm_proc/core/buses/spmi/src/platform/config/bear/9x05/spmi_sys.cfg"
<!-- System props -->
<props name="pmic_arb_base_addr" type=DALPROP_ATTR_TYPE_UINT32>
    0x02000000
</props>
#line 14 "/local/mnt/workspace/CRMBuilds/RPM.BF.2.1.3-00034-M9205AAAAANAZR-1_20191223_015705/b/rpm_proc/core/buses/spmi/src/platform/config/bear/9x05/spmi_rpm.xml"
        
        <!-- User configurable props -->
        <props name="owner" type=DALPROP_ATTR_TYPE_UINT32>
            4
        </props>
        <props name="interrupt" type=DALPROP_ATTR_TYPE_UINT32>
            48
        </props>
        
    </device>
</driver>
