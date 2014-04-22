<!-- NOTE: DAL config has trouble parsing multi-line comment, so please put -->
<!--       proper comment tags on each line                                 -->
<!--                                                                        -->
<!-- Clock Driver Properties file                                           -->
<!-- Note we depend on the include path for the following directories to be -->
<!-- available to the DAL config compiler:                                  -->
<!-- "${INC_ROOT}/core/systemdrivers/hal/clk/inc",                          -->
<!-- "${INC_ROOT}/core/systemdrivers/clock/src",                            -->
<!-- "${INC_ROOT}/core/systemdrivers/clock/config"                          -->
<!-- "${INC_ROOT}/core/systemdrivers/clock/config/${CHIPSET}"               -->
<driver name="Clock">
<global_def>
<!-- Min and MAx frequency (in Hz)for bimc clock  -->
  <var_seq name="bimc_gb" type=DALPROP_DATA_TYPE_UINT32_SEQ>
    0xFFFFFFFF, 0xFFFFFFFF,end
   </var_seq>
<!-- Min and MAx frequency (in Hz)for PCNOC clock  -->
  <var_seq name="pcnoc_gb" type=DALPROP_DATA_TYPE_UINT32_SEQ>
    0xFFFFFFFF, 0xFFFFFFFF,end
   </var_seq>
<!-- Min and MAx frequency (in Hz)for QPIC clock  -->
  <var_seq name="qpic_gb" type=DALPROP_DATA_TYPE_UINT32_SEQ>
    0xFFFFFFFF, 0xFFFFFFFF,end
  </var_seq>
<!-- Min and MAx frequency (in Hz)for RPM clock  -->
  <var_seq name="cpu_gb" type=DALPROP_DATA_TYPE_UINT32_SEQ>
    0xFFFFFFFF, 0xFFFFFFFF,end
  </var_seq>
<!-- To set all the RPM controlled clocks to voltage corner  -->
  <var_seq name="volt_gb" type=DALPROP_DATA_TYPE_UINT32_SEQ>
    0xFFFFFFFF,end
  </var_seq>
</global_def>
  <device id=DALDEVICEID_CLOCK>
<!-- Below properties are for getting the min/max limit as specified in ClockchipsetGlobal.xml -->
<!-- Property for querying bimc clock limit -->
<props name="lim_bimc" type=DALPROP_ATTR_TYPE_UINT32_SEQ_PTR>
  bimc_gb
</props>
<!-- Property for querying PCNOC clock limit -->
<props name="lim_pcnoc" type=DALPROP_ATTR_TYPE_UINT32_SEQ_PTR>
  pcnoc_gb
</props>
<!-- Property for querying QPIC clock limit -->
<props name="lim_qpic" type=DALPROP_ATTR_TYPE_UINT32_SEQ_PTR>
  qpic_gb
</props>
<!-- Property for querying RPM clock limit -->
<props name="lim_cpu" type=DALPROP_ATTR_TYPE_UINT32_SEQ_PTR>
  cpu_gb
</props>
<!-- Property for querying voltage corner -->
<props name="clk_volt_corn" type=DALPROP_ATTR_TYPE_UINT32_SEQ_PTR>
  volt_gb
</props>
<!-- Clock sources -->
<props name="ClockSources" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  SourceConfig
</props>
<!-- RPM Clocks -->
<props name="gcc_rpm_proc_hclk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  RPMClockConfig
</props>
<!-- Peripheral Config NOC Clocks -->
<props name="gcc_pcnoc_ahb_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  PCNOClockConfig
</props>
<!-- BIMC Clocks -->
<props name="gcc_bimc_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  BIMCClockConfig
</props>
<!-- APSS_AXI Clock -->
<props name="gcc_apss_axi_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  APSSAXIClockConfig
</props>
<!-- QDSS AT Clocks -->
<props name="gcc_qdss_at_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QDSSATClockConfig
</props>
<!-- QDSS Trace Clocks -->
<props name="gcc_qdss_traceclkin_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QDSSTraceClockConfig
</props>
<!-- QDSS STM Clocks -->
<props name="gcc_qdss_stm_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QDSSSTMClockConfig
</props>
<!-- QDSS TSCTR Clocks -->
<props name="gcc_qdss_tsctr_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QDSSTSCTRClockConfig
</props>
<!-- QDSS TRIG Clocks -->
<props name="gcc_qdss_trig_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QDSSTRIGClockConfig
</props>
<!-- RBCPR Clocks -->
<props name="gcc_rbcpr_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  RBCPRClockConfig
</props>
<!-- SPMI AHB Clocks -->
<props name="gcc_spmi_ahb_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  SPMIAHBClockConfig
</props>
<!-- SPMI SER Clocks -->
<props name="gcc_spmi_ser_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  SPMISERClockConfig
</props>
<!-- APSS AHB Clocks -->
<props name="gcc_apss_ahb_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  APSSAHBClockConfig
</props>
<!-- QPIC Clocks -->
<props name="gcc_qpic_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QPICClockConfig
</props>
 <!-- QPIC IO Clocks -->
<props name="gcc_qpic_io_macro_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  QPICIOClockConfig
</props>
<!-- CRYPTO Clocks -->
<props name="gcc_crypto_clk" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  CRYPTOClockConfig
</props>
<!-- Clock Log Defaults -->
<props name="ClockLogDefaults" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  ClockLogDefaultConfig
</props>
<!-- Clock Vreg level railway level mapping -->
<props name="ClockVregRailMap" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
  ClockVregRailMapConfig
</props>
  </device>
</driver>
