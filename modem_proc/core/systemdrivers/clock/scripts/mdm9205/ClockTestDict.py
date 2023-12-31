CLOCK_TEST_DICT = {
  0x20002: 'gcc_sys_noc_qdss_stm_axi_clk',
  0x20003: 'gcc_sys_noc_apss_ahb_clk',
  0x20008: 'gcc_pcnoc_ahb_clk',
  0x2000a: 'gcc_pcnoc_rpm_ahb_clk',
  0x2000b: 'gcc_pcnoc_tic_clk',
  0x2000c: 'gcc_pcnoc_at_clk',
  0x2000d: 'gcc_pcnoc_qdss_stm_axi_clk',
  0x2000e: 'gcc_pcnoc_apss_ahb_clk',
  0x20010: 'gcc_gp1_clk',
  0x20011: 'gcc_gp2_clk',
  0x20012: 'gcc_gp3_clk',
  0x20018: 'gcc_noc_conf_xpu_ahb_clk',
  0x20020: 'gcc_imem_axi_clk',
  0x20021: 'gcc_imem_cfg_ahb_clk',
  0x2002d: 'gcc_bimc_gfx_clk',
  0x20030: 'gcc_mss_cfg_ahb_clk',
  0x20031: 'gcc_mss_q6_bimc_axi_clk',
  0x20032: 'mss_gcc_dbg_clk',
  0x20038: 'gcc_rpm_cfg_xpu_clk',
  0x20040: 'gcc_qdss_dap_ahb_clk',
  0x20041: 'gcc_qdss_cfg_ahb_clk',
  0x20042: 'gcc_qdss_at_clk',
  0x20043: 'gcc_qdss_etr_usb_clk',
  0x20044: 'gcc_qdss_stm_clk',
  0x20045: 'gcc_qdss_traceclkin_clk',
  0x20046: 'gcc_qdss_tsctr_div2_clk',
  0x20048: 'gcc_qdss_tsctr_div3_clk',
  0x20049: 'gcc_qdss_dap_clk',
  0x2004a: 'gcc_qdss_tsctr_div4_clk',
  0x2004b: 'gcc_qdss_tsctr_div8_clk',
  0x2004c: 'gcc_qdss_tsctr_div16_clk',
  0x20050: 'gcc_apss_tcu_clk',
  0x20055: 'gcc_mss_tbu_axi_clk',
  0x20056: 'gcc_mss_tbu_gss_axi_clk',
  0x20057: 'gcc_mss_tbu_q6_axi_clk',
  0x20059: 'gcc_smmu_xpu_clk',
  0x2005b: 'gcc_smmu_cfg_clk',
  0x2005d: 'gcc_pcnoc_tbu_clk',
  0x2005f: 'gcc_smmu_cats_clk',
  0x20060: 'gcc_usb_hs_system_clk',
  0x20061: 'gcc_usb_hs_ahb_clk',
  0x20062: 'gcc_usb_hs_inactivity_timers_clk',
  0x20063: 'gcc_usb2a_phy_sleep_clk',
  0x20064: 'gcc_usb_hs_phy_cfg_ahb_clk',
  0x20068: 'gcc_sdcc1_apps_clk',
  0x20069: 'gcc_sdcc1_ahb_clk',
  0x20070: 'gcc_sdcc2_apps_clk',
  0x20071: 'gcc_sdcc2_ahb_clk',
  0x20078: 'gcc_qpic_clk',
  0x20079: 'gcc_qpic_ahb_clk',
  0x2007a: 'gcc_qpic_system_clk',
  0x20080: 'gcc_bimc_noc_mpu_ahb_clk',
  0x20088: 'gcc_blsp1_ahb_clk',
  0x20089: 'gcc_blsp1_sleep_clk',
  0x2008a: 'gcc_blsp1_qup1_spi_apps_clk',
  0x2008b: 'gcc_blsp1_qup1_i2c_apps_clk',
  0x2008c: 'gcc_blsp1_uart1_apps_clk',
  0x2008d: 'gcc_blsp1_uart1_sim_clk',
  0x2008e: 'gcc_blsp1_qup2_spi_apps_clk',
  0x20090: 'gcc_blsp1_qup2_i2c_apps_clk',
  0x20091: 'gcc_blsp1_uart2_apps_clk',
  0x20092: 'gcc_blsp1_uart2_sim_clk',
  0x20093: 'gcc_blsp1_qup3_spi_apps_clk',
  0x20094: 'gcc_blsp1_qup3_i2c_apps_clk',
  0x20095: 'gcc_blsp1_uart3_apps_clk',
  0x20094: 'gcc_blsp1_uart3_sim_clk',
  0x20095: 'gcc_blsp1_qup3_i2c_apps_clk',
  0x20098: 'gcc_blsp1_qup4_spi_apps_clk',
  0x20099: 'gcc_blsp1_qup4_i2c_apps_clk',
  0x2009a: 'gcc_blsp1_uart4_apps_clk',
  0x2009b: 'gcc_blsp1_uart4_sim_clk',
  0x2009c: 'gcc_blsp1_qup5_spi_apps_clk',
  0x2009d: 'gcc_blsp1_qup5_i2c_apps_clk',
  0x2009e: 'gcc_blsp1_uart5_apps_clk',
  0x200a0: 'gcc_blsp1_uart5_sim_clk',
  0x200a1: 'gcc_blsp1_qup6_spi_apps_clk',
  0x200a2: 'gcc_blsp1_qup6_i2c_apps_clk',
  0x200a3: 'gcc_blsp1_uart6_apps_clk',
  0x200a4: 'gcc_blsp1_uart6_sim_clk',
  0x200bd: 'gcc_vfe_qdss_at_clk',
  0x200be: 'gcc_vfe_qdss_tsctr_div8_clk',
  0x200c8: 'gcc_prng_xpu_cfg_ahb_clk',
  0x200c9: 'gcc_pcnoc_mpu_cfg_ahb_clk',
  0x200d0: 'gcc_pdm_ahb_clk',
  0x200d1: 'gcc_pdm_xo4_clk',
  0x200d2: 'gcc_pdm2_clk',
  0x200d8: 'gcc_prng_ahb_clk',
  0x200e9: 'gcc_apss_tcu_async_clk',
  0x200ea: 'gcc_ultaudio_sec_pcm_data_oe_clk',
  0x200eb: 'gcc_ultaudio_ter_pcm_data_oe_clk',
  0x200f0: 'gcc_tcsr_ahb_clk',
  0x200f8: 'gcc_boot_rom_ahb_clk',
  0x20100: 'gcc_msg_ram_ahb_clk',
  0x20108: 'gcc_tlmm_ahb_clk',
  0x20109: 'gcc_tlmm_clk',
  0x20110: 'gcc_mpm_ahb_clk',
  0x20118: 'gcc_rpm_proc_fclk',
  0x20119: 'gcc_rpm_proc_hclk',
  0x2011a: 'gcc_rpm_bus_ahb_clk',
  0x2011b: 'gcc_rpm_sleep_clk',
  0x2011c: 'gcc_rpm_timer_clk',
  0x20120: 'gcc_sec_ctrl_acc_clk',
  0x20121: 'gcc_sec_ctrl_ahb_clk',
  0x20122: 'gcc_sec_ctrl_clk',
  0x20123: 'gcc_sec_ctrl_sense_clk',
  0x20124: 'gcc_sec_ctrl_boot_rom_patch_clk',
  0x20128: 'gcc_spmi_ser_clk',
  0x20129: 'gcc_spmi_pcnoc_ahb_clk',
  0x2012a: 'gcc_spmi_ahb_clk',
  0x20130: 'gcc_spdm_cfg_ahb_clk',
  0x20131: 'gcc_spdm_mstr_ahb_clk',
  0x20132: 'gcc_spdm_ff_clk',
  0x20133: 'gcc_spdm_bimc_cy_clk',
  0x20134: 'gcc_spdm_snoc_cy_clk',
  0x20135: 'gcc_spdm_pcnoc_cy_clk',
  0x20136: 'gcc_spdm_rpm_cy_clk',
  0x20138: 'gcc_crypto_clk',
  0x20139: 'gcc_crypto_axi_clk',
  0x2013a: 'gcc_crypto_ahb_clk',
  0x20140: 'gcc_spdm_csi0_clk',
  0x20141: 'gcc_spdm_pclk0_clk',
  0x20142: 'gcc_spdm_ahb_clk',
  0x20143: 'gcc_spdm_gfx3d_clk',
  0x20144: 'gcc_spdm_vfe0_clk',
  0x20145: 'gcc_spdm_vcodec0_clk',
  0x20146: 'gcc_spdm_mdp_clk',
  0x20148: 'gcc_ahb_clk',
  0x20149: 'gcc_xo_clk',
  0x2014a: 'gcc_xo_div4_clk',
  0x2014b: 'gcc_im_sleep_clk',
  0x20150: 'gcc_bimc_xo_clk',
  0x20151: 'gcc_bimc_cfg_ahb_clk',
  0x20152: 'gcc_bimc_sleep_clk',
  0x20153: 'gcc_bimc_pcnoc_axi_clk',
  0x20154: 'gcc_bimc_clk',
  0x20155: 'gcc_bimc_apss_axi_clk',
  0x20156: 'gcc_bimc_at_clk',
  0x20157: 'gcc_mcdma_axi_clk',
  0x20158: 'gcc_nav_axi_clk',
  0x20159: 'gcc_bimc_tcu_clk',
  0x2015a: 'gcc_pcnoc_ddr_cfg_clk',
  0x2015b: 'gcc_ddr_dim_sleep_clk',
  0x20162: 'gcc_ultaudio_pcnoc_mport_clk',
  0x20163: 'gcc_ultaudio_pcnoc_sway_clk',
  0x20168: 'gcc_apss_ahb_clk',
  0x20169: 'gcc_apss_axi_clk',
  0x2016a: 'apss_gcc_dbg_clk',
  0x20178: 'gcc_pcnoc_bus_timeout0_ahb_clk',
  0x20179: 'gcc_pcnoc_bus_timeout1_ahb_clk',
  0x2017a: 'gcc_pcnoc_bus_timeout2_ahb_clk',
  0x2017b: 'gcc_pcnoc_bus_timeout3_ahb_clk',
  0x2017c: 'gcc_pcnoc_bus_timeout4_ahb_clk',
  0x20180: 'gcc_pcnoc_bus_timeout5_ahb_clk',
  0x20181: 'gcc_pcnoc_bus_timeout6_ahb_clk',
  0x20182: 'gcc_pcnoc_bus_timeout7_ahb_clk',
  0x20183: 'gcc_pcnoc_bus_timeout8_ahb_clk',
  0x20184: 'gcc_pcnoc_bus_timeout9_ahb_clk',
  0x20188: 'gcc_dehr_clk',
  0x20190: 'gcc_rbcpr_clk',
  0x20191: 'gcc_rbcpr_ahb_clk',
  0x20198: 'gcc_usb_hsic_ahb_clk',
  0x20199: 'gcc_usb_hsic_system_clk',
  0x2019a: 'gcc_usb_hsic_clk',
  0x2019b: 'gcc_usb_hsic_io_cal_clk',
  0x2019c: 'gcc_usb_hsic_io_cal_sleep_clk',
  0x201a0: 'usb2_phy_gcc_atest_ulpi_0_clk',
  0x201a8: 'usb2_phy_gcc_atest_ulpi_1_clk',
  0x201b0: 'gcc_ultaudio_ahbfabric_ixfabric_clk',
  0x201b1: 'gcc_ultaudio_ahbfabric_ixfabric_lpm_clk',
  0x201b2: 'gcc_ultaudio_lpaif_pri_i2s_clk',
  0x201b3: 'gcc_ultaudio_lpaif_sec_i2s_clk',
  0x201b4: 'gcc_ultaudio_lpaif_aux_i2s_clk',
  0x201b4: 'gcc_ultaudio_pcm_data_oe_clk',
  0x201b4: 'gcc_ultaudio_ext_mclk2_clk',
  0x201b5: 'gcc_ultaudio_avsync_xo_clk',
  0x201b6: 'gcc_ultaudio_stc_xo_clk',
  0x201b7: 'gcc_codec_digcodec_clk',
  0x201b8: 'gcc_emac_0_axi_clk',
  0x201b9: 'gcc_emac_0_ahb_clk',
  0x201ba: 'gcc_emac_0_sys_25m_clk',
  0x201bb: 'gcc_emac_0_tx_clk',
  0x201bc: 'gcc_emac_0_125m_clk',
  0x201bd: 'gcc_emac_0_rx_clk',
  0x201be: 'gcc_emac_0_sys_clk',
  0x201c0: 'gpll0_dtest',
  0x201c1: 'gpll0_lock_det',
  0x201c2: 'gpll1_dtest',
  0x201c3: 'gpll1_lock_det',
  0x201c4: 'bimc_pll_dtest',
  0x201c5: 'bimc_pll_lock_det',
  0x201c8: 'mpm_gcc_temp_sensor_ringosc_clk',
  0x201d0: 'apss_gcc_ringosc_clk',
  0x201e0: 'gcc_dcd_xo_clk',
  0x50001: 'clk_xo_div12',
  0x50002: 'clk_uim0',
  0x50003: 'clk_uim1',
  0x50004: 'clk_xo_uim0',
  0x50005: 'clk_xo_uim1',
  0x50007: 'clk_bit_coxm',
  0x5000b: 'pll_lock_det_mpll1',
  0x5000c: 'pll_lock_det_mpll2',
  0x5000d: 'clk_q6_mon',
  0x5000e: 'clk_bus_bridge_mon',
  0x5000f: 'clk_uim_div',
  0x50011: 'clk_q6',
  0x50013: 'mpll1_dtest',
  0x50014: 'mpll2_dtest',
  0x50019: 'mpll1_main_modem_cx_to_ls',
  0x5001a: 'mpll1_early_div3_modem_cx_to_ls',
  0x5001b: 'mpll1_early_div5',
  0x50021: 'clk_bus_modem_bridge_cx',
  0x50022: 'clk_bus_modem_cx_to_ls',
  0x50023: 'clk_bus_nc_hm_bridge_cx',
  0x50025: 'mpll1_early_div3_nav_cx_to_ls',
  0x50026: 'mpll1_main_nav_cx_to_ls',
  0x5002a: 'clk_modem_snoc_axi',
  0x5002b: 'clk_nav_snoc_axi',
  0x5002c: 'clk_modem_cfg_ahb',
  0x5002d: 'clk_bus_combodac_comp',
  0x5002e: 'clk_bus_csr',
  0x5002f: 'clk_bus_bridge',
  0x50030: 'clk_bus_coxm',
  0x50031: 'clk_bus_slave_timeout',
  0x50032: 'bus_slave_timeout_time_base',
  0x50033: 'clk_bus_atb',
  0x50034: 'clk_bus_uim0',
  0x50035: 'clk_bus_uim1',
  0x50037: 'clk_bit_coxm',
  0x5003b: 'clk_bus_q6',
  0x5003d: 'clk_dac0',
  0x5003e: 'mpll1_out_early_div5',
  0x5003f: 'clk_dac1',
}