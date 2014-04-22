#include "ddr_phy_training.h"
#include "ddr_params.h"
#include "HAL_DDR.h"
#include "ddr_log.h"
#define TRAINING_HISTOGRAM_ENABLE 0

uint16 ca_training_pattern[CA_PATTERN_NUM][6] = {
  { 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001 }, 
  { 0x03FF, 0x03FF, 0x0000, 0x03FF, 0x0000, 0x0000 }, 
  { 0x0155, 0x0155, 0x02aa, 0x0155, 0x02aa, 0x02aa }, 
  { 0x02aa, 0x02aa, 0x0155, 0x02aa, 0x0155, 0x0155 }};
	
	/*
  { 0x0000, 0x0000, 0x03FF, 0x0000, 0x03FF, 0x03FF }, 
  { 0x03FF, 0x03FF, 0x0000, 0x03FF, 0x0000, 0x0000 }, 
  { 0x0155, 0x0155, 0x02aa, 0x0155, 0x02aa, 0x02aa }, 
  { 0x02aa, 0x02aa, 0x0155, 0x02aa, 0x0155, 0x0155 }};*/
  

 uint32 dq_training_pattern[] =
{
0xDEADBEEF, 0x5A5A5A5A, 0xA5A5A5A5, 0xFEEDFACE, 0xCAFEBABE, 0xA5A5A5A5, 0x5A5A5A5A, 0x0BADF00D, // Tom's Pattern 
0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0x0f0f0f0f, 0xf0f0f0f0, 0x00000000, 0xffffffff, // Yong's Pattern 
0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, // Mike/Scott's Pattern 
0x21524110, 0xA5A5A5A5, 0x5A5A5A5A, 0x01120531, 0x35014541, 0x5A5A5A5A, 0xA5A5A5A5, 0xF4520FF2, // Tom's Pattern inverted 
0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0xf0f0f0f0, 0x0f0f0f0f, 0xffffffff, 0x00000000, // Yong's Pattern inverted 
0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, // Mike/Scott's Pattern inverted 
};

uint8 fail_count_histogram_perbyte[PHY_DQ_MAX_NUM][FAIL_HISTOGRAM_SIZE]={{0}};
 
uint16 DDRSS_ddr_phy_dq_wr_training_ddrss( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 rank, 
uint16 max_loop_count, uint16 coarse_init_val, uint16 dq_write_training[PHY_DQ_MAX_NUM], uint32 clk_freq_in_khz ){
uint32 CDC_INIT_VALUE =  0x174,coarse_sweep=0,
       //pass_left_edge=0,pass_right_edge=0,
	   pass_byte_mid_cdc=0;
uint8

    
    loop_count=0,
	pass_min_edge[PHY_DQ_MAX_NUM]={0},
	pass_max_edge[PHY_DQ_MAX_NUM]={0},
	num_training_loop=0;
	
  uint32
   max_coarse=0, 
   //training_result_per_byte=0,
    read_data[DQ_TRAINING_PATTERN_SIZE]={0},dq;
 uint16     training_period_1ps=0;
	 training_period_1ps = 1000000000/clk_freq_in_khz;
  max_coarse = ((training_period_1ps / 2));
  
  if(coarse_init_val > MAX_COARSE_STEP)
  {
	 coarse_init_val = 0xA; // coarse init value should be 0xA for a proper sweep.
  }
  
  for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
	{
		if(cdc_lut[coarse_sweep]> max_coarse) break;
	}
	
	max_coarse = coarse_sweep ;
  
  //histogram_size = max_coarse * MAX_FINE_STEP;
  if(max_loop_count > 255) max_loop_count = 255;

  clear_histogram();
  
 if (coarse_init_val > 0)
 {
  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
		HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(  _inst_, interface,   CDC_INIT_VALUE, dq);
		fail_count_histogram_perbyte[dq][coarse_init_val-1]=0x1;
  }
 }
 
  for(num_training_loop = 0; num_training_loop < 2 ; num_training_loop++ ) {
	 for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
	{
		for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++)
		{
		
			  HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(_inst_, interface, cdc_lut[coarse_sweep], dq);
			  for(loop_count = 0; loop_count < max_loop_count; loop_count++)
			  {
				  ddr_mem_write_high_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
				  ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
				  //training_result_per_byte = 
				  DDRSS_dq_rd_wr_training_histogram_update(interface, coarse_sweep, read_data, dq_training_pattern, fail_count_histogram_perbyte,dq);
			  }
		} // coarse_sweep
		
		DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[dq], &pass_max_edge[dq], fail_count_histogram_perbyte[dq],coarse_init_val,max_coarse);
		pass_byte_mid_cdc = (cdc_lut[pass_min_edge[dq]]+cdc_lut[pass_max_edge[dq]])/2;
		dq_write_training[dq] = pass_byte_mid_cdc;
		HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(_inst_, interface, pass_byte_mid_cdc, dq);
	}
}

	#ifdef TRAINING_HISTOGRAM_ENABLE
	
		ddr_printf (DDR_STATUS, "WR CDC Result = Zero[PASS] / NotZero[FAIL]\n");
	
		for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++) {
			ddr_printf (DDR_STATUS, "WR CDC on DQ %d\n",dq);
			ddr_printf (DDR_STATUS, "CDC Delay (in ps) - Result\n");

			for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++) {
				ddr_printf (DDR_STATUS, "%d - %d\n", cdc_lut[coarse_sweep], fail_count_histogram_perbyte[dq][coarse_sweep]);
			}
		}
	
	#endif
	
return 0;
}

uint16 DDRSS_ddr_phy_dq_rd_training_ddrss( uint32 _inst_, SDRAM_INTERFACE interface, uint16 rank, 
uint16 max_loop_count, uint16 coarse_init_val, uint16 dq_read_training[PHY_DQ_MAX_NUM],uint32 clk_freq_in_khz ){
uint32 CDC_INIT_VALUE =  0x174,coarse_sweep=0,
       //pass_left_edge=0,pass_right_edge=0,
	   pass_byte_mid_cdc=0;

uint8

    loop_count=0,
	
	pass_min_edge[PHY_DQ_MAX_NUM]={0},
	pass_max_edge[PHY_DQ_MAX_NUM]={0},
	num_training_loop=0;

  uint32
    //training_result_per_byte=0,
	max_coarse=0,
    read_data[DQ_TRAINING_PATTERN_SIZE]={0},dq;
	 uint16     training_period_1ps=0;
  training_period_1ps = 1000000000/clk_freq_in_khz;
  max_coarse=training_period_1ps/2;
  if(coarse_init_val > MAX_COARSE_STEP)
  {
	 coarse_init_val= 0xA ;//coarse_init_val should be 10.
  }
  
  for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
	{
		if(cdc_lut[coarse_sweep]> max_coarse) break;
	}
	
	max_coarse =coarse_sweep ;
 
  if(max_loop_count > 255) max_loop_count = 255;

  clear_histogram();
  
  ddr_mem_write_low_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);

 if (coarse_init_val > 0)
 {	 
  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
		HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(  _inst_, interface,   CDC_INIT_VALUE, dq);
		fail_count_histogram_perbyte[dq][coarse_init_val-1]=0x1;
  }
 }
 
  for(num_training_loop = 0; num_training_loop < 2 ; num_training_loop++ ) {
	 for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
	{
		for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++)
		{
		
			  HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(_inst_, interface, cdc_lut[coarse_sweep], dq);
			  for(loop_count = 0; loop_count < max_loop_count; loop_count++)
			  {
				  ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
				  //training_result_per_byte = 
				  DDRSS_dq_rd_wr_training_histogram_update(interface, coarse_sweep, read_data, dq_training_pattern, fail_count_histogram_perbyte,dq);
			  }
		} // coarse_sweep
		
		DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[dq], &pass_max_edge[dq], fail_count_histogram_perbyte[dq],coarse_init_val,max_coarse);
		pass_byte_mid_cdc = (cdc_lut[pass_min_edge[dq]]+cdc_lut[pass_max_edge[dq]])/2;
		dq_read_training[dq] = pass_byte_mid_cdc;
		HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(_inst_, interface, pass_byte_mid_cdc, dq);

	}
}

	#ifdef TRAINING_HISTOGRAM_ENABLE
	
	ddr_printf (DDR_STATUS, "RD CDC Result = Zero[PASS] / NotZero[FAIL]\n");

	for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++) {
		ddr_printf (DDR_STATUS, "RD CDC on DQ %d\n",dq);
		ddr_printf (DDR_STATUS, "CDC Delay (in ps) - Result\n");

		for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++) {
			ddr_printf (DDR_STATUS, "%d - %d\n", cdc_lut[coarse_sweep], fail_count_histogram_perbyte[dq][coarse_sweep]);
		}
	}
	
	#endif
	
return 0;
}

uint16 HAL_SDRAM_ddr_phy_ca_training( uint32 _inst_, SDRAM_INTERFACE interface, uint16 chip_select,
uint16 max_loop_count,uint16 coarse_init_val, uint16 *ca_training_full_cdc , uint32 clk_freq_in_khz){

uint32     ca_training_pattern_result_read=0,CDC_INIT_VALUE =  0x174, byte_mid=0,coarse_sweep=0;
uint8
    pattern_num=0,
    ca_mapping=0,
    pass_min_edge[2]={0},
    pass_max_edge[2]={0},
	first_rank=0,
	last_rank=0,rank=0;
  uint16
    ca_training_expected=0,
    cs_rise_pat=0,
    cs_fall_pat=0,
	//fail=0,
    training_period_1ps=0;
  uint32     max_coarse=0,loop_count=0,pattern_num_max=4;

	 training_period_1ps = 1000000000/clk_freq_in_khz;
  max_coarse = ((training_period_1ps / 2));
  
  if(coarse_init_val > MAX_COARSE_STEP)
  {
	 coarse_init_val= 0xA ;//coarse_init_val should be 10.
  }
  
  for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
	{
		if(cdc_lut[coarse_sweep]> max_coarse) break;
	}
	
	max_coarse = coarse_sweep ;
	
  if(max_coarse > MAX_COARSE_STEP)
  {
	  max_coarse = MAX_COARSE_STEP;
  }
	
  clear_histogram();
	
 if (coarse_init_val > 0)
 {
	//  for(coarse_sweep=0;coarse_sweep<coarse_init_val;coarse_sweep++){
		fail_count_histogram_perbyte[0][coarse_init_val-1]=0x1;
 }
   
	if ( chip_select == SDRAM_BOTH ){//dual rank
		first_rank = SDRAM_CS0;
		last_rank  = SDRAM_CS1;
	}
	else {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
		 first_rank = chip_select;
		 last_rank  = chip_select;
	}
 
 for(rank=first_rank;rank<=last_rank;rank++)
 {
	 	HAL_SDRAM_DDR_PHY_CA_training_mode(_inst_, interface, 1); //1: for entry, 0: for exit
		HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(_inst_, interface, CDC_INIT_VALUE);
 
    for(pattern_num = 0; pattern_num < pattern_num_max;  pattern_num ++)
	{
		//printf("Pattern : %d\n", pattern_num);
      for(ca_mapping = 0; ca_mapping < 2; ca_mapping ++)
	    {                
		  HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(_inst_, interface, CDC_INIT_VALUE);
		  HAL_SDRAM_BIMC_CA_Training_Exit(_inst_ , interface, rank);
          HAL_SDRAM_BIMC_CA_Training_Pattern_Set(_inst_, 
                                        interface,
                                        rank, 
                                        ca_mapping,
                                        ca_training_pattern[pattern_num][PRECS_RISE], 
                                        ca_training_pattern[pattern_num][PRECS_FALL], 
                                        ca_training_pattern[pattern_num][CS_RISE], 
                                        ca_training_pattern[pattern_num][CS_FALL], 
                                        ca_training_pattern[pattern_num][POSTCS_RISE], 
                                        ca_training_pattern[pattern_num][POSTCS_FALL]);
         cs_rise_pat = ca_training_pattern[pattern_num][CS_RISE];
         cs_fall_pat = ca_training_pattern[pattern_num][CS_FALL];
         ca_training_expected = DDRSS_get_ca_exp_pattern(_inst_ , cs_rise_pat, cs_fall_pat, ca_mapping);

		 //printf ("CA_mapping: %d\n ",ca_mapping);
         for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++){
          //printf("CDC delay value: %d\n ",coarse_sweep );
			  HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(_inst_, interface, cdc_lut[coarse_sweep]);
              for(loop_count = 0; loop_count < max_loop_count; loop_count++)             {
				  
                HAL_SDRAM_BIMC_CA_Training_Pattern_Toggle(_inst_ , interface);
                ca_training_pattern_result_read = HAL_SDRAM_DDR_PHY_read_dq_training_status_for_ca_training(_inst_, interface);
                if(ca_mapping == 1) {ca_training_pattern_result_read &= 0x0303;}
               /* fail=DDRSS_ca_training_histogram_update(ca_mapping, 
					                                     coarse_sweep, 
					                                     ca_training_pattern_result_read, ca_training_expected ); //ngamini - update based on remap table*/
														 
			    if(ca_training_pattern_result_read!=ca_training_expected) fail_count_histogram_perbyte[0][coarse_sweep]+=1;
 
             } // loop_count
         } // coarse_sweep
        } // ca_mapping
    } // pattern_num
	
	HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(_inst_, interface, CDC_INIT_VALUE);

    HAL_SDRAM_BIMC_CA_Training_Exit(_inst_ , interface, rank);
	HAL_SDRAM_DDR_PHY_CA_training_mode(_inst_, interface, 0); //1: for entry, 0: for exit

}
	DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[0], &pass_max_edge[0], fail_count_histogram_perbyte[0],coarse_init_val,max_coarse);
	
	#ifdef TRAINING_HISTOGRAM_ENABLE

		ddr_printf (DDR_STATUS, "CA CDC Result = Zero[PASS] / NotZero[FAIL]\n");
		ddr_printf (DDR_STATUS, "CDC Delay (in ps) - Result\n");
		
		for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++) {
			ddr_printf (DDR_STATUS, "%d - %d\n", cdc_lut[coarse_sweep], fail_count_histogram_perbyte[0][coarse_sweep]);
		}
		
	#endif
	
	byte_mid = (cdc_lut[pass_min_edge[0]]+cdc_lut[pass_max_edge[0]])/2;
    *ca_training_full_cdc=byte_mid;
    HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(_inst_, interface, byte_mid);
	
//return ca_training_full_cdc;
return 0;

}

void HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config)
{
  uint32 phy_offset, ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
  //uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Update CA CDC delay */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset, HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL), config << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));

  /* Issue manual CDCCAL */
  HAL_SDRAM_PHY_Manual_CDC_Cal(base, interface, ddr_type);

  /* Enable auto CDCCAL */
  HAL_SDRAM_PHY_Enable_CDC_Cal(base, interface);

} /* HAL_SRAM_DDR_PHY_Update_CA_CDC_Config */

void HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config, uint32 which_dq)
{
  uint32 phy_offset, ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
  //uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

//  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
 // {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * which_dq;
    
    /* Update DQ read CDC delay */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                   config << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));
				 

  /* Issue manual CDCCAL */
  HAL_SDRAM_PHY_Manual_CDC_Cal(base, interface, ddr_type);

  /* Enable auto CDCCAL */
  HAL_SDRAM_PHY_Enable_CDC_Cal(base, interface);
  //}

} /* HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config */

void HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config, uint32 which_dq)
{
  uint32 phy_offset, ddr_type = ddr_get_params(SDRAM_INTERFACE_0)->common.device_type;
  //uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  //for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  //{
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * which_dq;

    /* Update DQ write CDC delay */
	// HWIO_OUTXF(  offset,DIM_C00_DIM_CA_CDC_DELAY_CFG,DELAY_VAL,config);
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                   config << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));

  /* Issue manual CDCCAL */
  HAL_SDRAM_PHY_Manual_CDC_Cal(base, interface, ddr_type);

  /* Enable auto CDCCAL */
  HAL_SDRAM_PHY_Enable_CDC_Cal(base, interface);

} /* HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config */

void HAL_SDRAM_DDR_PHY_CA_training_mode(uint32 base, SDRAM_INTERFACE interface, uint32 mode)
{
	
	uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);
if(mode==1){
  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;

    /* Update DQ write CDC delay */
	// HWIO_OUTXF(  offset,DIM_C00_DIM_CA_CDC_DELAY_CFG,DELAY_VAL,config);
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(TOP_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_SEL),
                   1 << HWIO_SHFT(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_SEL));
	  BOOT_HWIO_OUTM(PHY_DQ_ADDR(TOP_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_EN),
                   0 << HWIO_SHFT(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_EN));
				   
  }			   
}

else if(mode ==0){
	  for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + PHY_DQ1_OFFSET * dq;

    /* Update DQ write CDC delay */
	// HWIO_OUTXF(  offset,DIM_C00_DIM_CA_CDC_DELAY_CFG,DELAY_VAL,config);
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(TOP_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_SEL),
                   0 << HWIO_SHFT(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_SEL));
	  BOOT_HWIO_OUTM(PHY_DQ_ADDR(TOP_CFG), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_EN),
                   0 << HWIO_SHFT(PHY_DQ_ADDR(TOP_CFG), DQ_LB_MODE_EN));
				   
  }	
}

}

uint32 HAL_SDRAM_DDR_PHY_read_dq_training_status_for_ca_training(uint32 base, SDRAM_INTERFACE interface)
{
	uint16 remapped_byte[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8 dq_0=0;
	uint8 dq_1=0;
	uint32 phy_offset;
	uint32 final_remap_1 =0;
	int i =0;

  /* Configure for correct offset */
  if(interface == SDRAM_INTERFACE_0)
  {
    phy_offset = SDRAM_0_OFFSET;
  }
  else
  {
    phy_offset = SDRAM_1_OFFSET;
  }
 
	dq_0 = BOOT_HWIO_IN(PHY_DQ_ADDR(DQ_SMT_STATUS),phy_offset+PHY_DQ1_OFFSET*0);
	dq_1 = BOOT_HWIO_IN(PHY_DQ_ADDR(DQ_SMT_STATUS),phy_offset+PHY_DQ1_OFFSET*1);
				
			/*remapped_byte[ 2] = ( dq_0 & 0x80 )?1:0;
			remapped_byte[ 0] = ( dq_0 & 0x40 )?1:0;
			remapped_byte[ 1] = ( dq_0 & 0x20 )?1:0;
			remapped_byte[ 3] = ( dq_0 & 0x10 )?1:0;
			remapped_byte[ 4] = ( dq_0 & 0x08 )?1:0;
			remapped_byte[ 6] = ( dq_0 & 0x04 )?1:0;
			remapped_byte[ 5] = ( dq_0 & 0x02 )?1:0;
			remapped_byte[ 7] = ( dq_0 & 0x01 )?1:0;
			
			remapped_byte[15] = ( dq_1 & 0x80 )?1:0;
			remapped_byte[14] = ( dq_1 & 0x40 )?1:0;
			remapped_byte[13] = ( dq_1 & 0x20 )?1:0;
			remapped_byte[12] = ( dq_1 & 0x10 )?1:0;
			remapped_byte[11] = ( dq_1 & 0x08 )?1:0;
			remapped_byte[10] = ( dq_1 & 0x04 )?1:0;
			remapped_byte[9] = ( dq_1 & 0x02 )?1:0;
			remapped_byte[8] = ( dq_1 & 0x01 )?1:0;*/
			
			
			/*remapped_byte[16] = ( dq_2 & 0x80 )?1:0;
			remapped_byte[17] = ( dq_2 & 0x40 )?1:0;
			remapped_byte[18] = ( dq_2 & 0x20 )?1:0;
			remapped_byte[19] = ( dq_2 & 0x10 )?1:0;
			remapped_byte[20] = ( dq_2 & 0x08 )?1:0;
			remapped_byte[21] = ( dq_2 & 0x04 )?1:0;
			remapped_byte[22] = ( dq_2 & 0x02 )?1:0;
			remapped_byte[23] = ( dq_2 & 0x01 )?1:0;
			
			
			remapped_byte[28] = ( dq_3 & 0x80 )?1:0;
			remapped_byte[31] = ( dq_3 & 0x40 )?1:0;
			remapped_byte[30] = ( dq_3 & 0x20 )?1:0;
			remapped_byte[29] = ( dq_3 & 0x10 )?1:0;
			remapped_byte[27] = ( dq_3 & 0x08 )?1:0;
			remapped_byte[26] = ( dq_3 & 0x04 )?1:0;
			remapped_byte[25] = ( dq_3 & 0x02 )?1:0;
			remapped_byte[24] = ( dq_3 & 0x01 )?1:0;*/
			
			
			remapped_byte[ 7] = ( dq_0 & 0x01 )?1:0;
			remapped_byte[ 6] = ( dq_0 & 0x02 )?1:0;
			remapped_byte[ 5] = ( dq_0 & 0x04 )?1:0;
			remapped_byte[ 4] = ( dq_0 & 0x08 )?1:0;
			remapped_byte[ 3] = ( dq_0 & 0x10 )?1:0;
			remapped_byte[ 2] = ( dq_0 & 0x20 )?1:0;
			remapped_byte[ 1] = ( dq_0 & 0x40 )?1:0;
			remapped_byte[ 0] = ( dq_0 & 0x80 )?1:0;
			
			remapped_byte[8] = ( dq_1 & 0x01 )?1:0;
			remapped_byte[9] = ( dq_1 & 0x02 )?1:0;
			remapped_byte[10] = ( dq_1 & 0x04 )?1:0;
			remapped_byte[11] = ( dq_1 & 0x08 )?1:0;
			remapped_byte[12] = ( dq_1 & 0x10 )?1:0;
			remapped_byte[13] = ( dq_1 & 0x20 )?1:0;
			remapped_byte[14] = ( dq_1 & 0x40 )?1:0;
			remapped_byte[15] = ( dq_1 & 0x80 )?1:0;
			
			/*
			remapped_byte[16] = ( dq_2 & 0x01 )?1:0;
			remapped_byte[17] = ( dq_2 & 0x02 )?1:0;
			remapped_byte[18] = ( dq_2 & 0x04 )?1:0;
			remapped_byte[19] = ( dq_2 & 0x08 )?1:0;
			remapped_byte[20] = ( dq_2 & 0x10 )?1:0;
			remapped_byte[21] = ( dq_2 & 0x20 )?1:0;
			remapped_byte[22] = ( dq_2 & 0x40 )?1:0;
			remapped_byte[23] = ( dq_2 & 0x80 )?1:0;
			
			remapped_byte[24] = ( dq_3 & 0x01 )?1:0;
			remapped_byte[25] = ( dq_3 & 0x02 )?1:0;
			remapped_byte[26] = ( dq_3 & 0x04 )?1:0;
			remapped_byte[27] = ( dq_3 & 0x08 )?1:0;
			remapped_byte[28] = ( dq_3 & 0x10 )?1:0;
			remapped_byte[29] = ( dq_3 & 0x20 )?1:0;
			remapped_byte[30] = ( dq_3 & 0x40 )?1:0;
			remapped_byte[31] = ( dq_3 & 0x80 )?1:0;*/
            
			for(i=0;i<16;i++){
				final_remap_1 = final_remap_1 | (remapped_byte[i]<<i); 
			}
			
			return (final_remap_1);
			  //printf("DQ0,DQ1,DQ2,DQ3 : 0x%x \n",final_remap);
}

uint16 DDRSS_ca_training_histogram_update(uint8 ca_mapping, uint8 delay_step, uint16 read, uint16 expected)
{

 uint16 fail = 0,fail_count_histogram_perbit[10][FAIL_HISTOGRAM_SIZE]={{0}};
  
  if(ca_mapping == 0)
  {
    if((read & 0x0003) != (expected & 0x0003)) {fail_count_histogram_perbit[0][delay_step] += 1; fail |= 0x0001;}
    if((read & 0x000C) != (expected & 0x000C)) {fail_count_histogram_perbit[1][delay_step] += 1; fail |= 0x0002;}
    if((read & 0x0030) != (expected & 0x0030)) {fail_count_histogram_perbit[2][delay_step] += 1; fail |= 0x0004;}
    if((read & 0x00C0) != (expected & 0x00C0)) {fail_count_histogram_perbit[3][delay_step] += 1; fail |= 0x0008;}
    if((read & 0x0300) != (expected & 0x0300)) {fail_count_histogram_perbit[5][delay_step] += 1; fail |= 0x0020;}
    if((read & 0x0C00) != (expected & 0x0C00)) {fail_count_histogram_perbit[6][delay_step] += 1; fail |= 0x0040;}
    if((read & 0x3000) != (expected & 0x3000)) {fail_count_histogram_perbit[7][delay_step] += 1; fail |= 0x0080;}
    if((read & 0xC000) != (expected & 0xC000)) {fail_count_histogram_perbit[8][delay_step] += 1; fail |= 0x0100;}
  }
  
  if (ca_mapping != 0){
    if((read & 0x0003) != (expected & 0x0003)) {fail_count_histogram_perbit[4][delay_step] += 1; fail |= 0x0010;}
    if((read & 0x0300) != (expected & 0x0300)) {fail_count_histogram_perbit[9][delay_step] += 1; fail |= 0x0200;}
  }
  
  
  return fail;
	
  
}
uint16 DDRSS_get_ca_exp_pattern(uint32 _inst_, uint16 ca_pat_rise, uint16 ca_pat_fall, uint16 ca_train_mapping)
{

uint16 i;
	uint16 index0;
	uint16 index1;
	uint16 exp_pattern_ph0;
	uint16 exp_pattern_ph1;

	index0 = 0;
	index1 = 0;
	exp_pattern_ph0 = 0;
	exp_pattern_ph1 = 0;
	
	for (i = 0; i <= 9;  i ++)
  {
		if (i != 4 && i != 9 && ca_train_mapping == 0)
    {
			exp_pattern_ph0 = exp_pattern_ph0 | (((ca_pat_rise >> i) & 0x1) << index0);
			exp_pattern_ph0 = exp_pattern_ph0 | (((ca_pat_fall >> i) & 0x1) << (index0 + 1));
			index0 = index0 + 2;
		}
		else
		{
			if(ca_train_mapping == 1 && (i == 4 || i == 9))
      {
				exp_pattern_ph1 = exp_pattern_ph1 | (((ca_pat_rise >> i) & 0x1) << index1);
				exp_pattern_ph1 = exp_pattern_ph1 | (((ca_pat_fall >> i) & 0x1) << (index1 + 1));
				index1 = index1 + 8;
			}
		}
	}
	
	if(ca_train_mapping == 0)
  {
		return exp_pattern_ph0;
	}
	else
	{
		return exp_pattern_ph1;
	}
}
void DDRSS_histogram_min_max_scan(uint32 _inst_, uint8 *min, uint8 *max, uint8 fail_count_histogram[FAIL_HISTOGRAM_SIZE],uint32 coarse_init_val,uint32 histogram_size){

	uint8 lenght_cur = 0;
	uint8 min_cur = 0;
	uint8 max_cur = 0;
	uint8 lenght_max = 0;
	uint8 sweep;

	*max = 0;
	*min = 0;
	
	for(sweep = 1; sweep < histogram_size - 1; sweep++)
  {
		if((fail_count_histogram[sweep - 1] != 0) && (fail_count_histogram[sweep] == 0))
    {
			min_cur = sweep;
			max_cur = sweep;
			lenght_cur = 0;
		}
		if((fail_count_histogram[sweep] == 0) && (fail_count_histogram[sweep + 1] != 0))
    {
			max_cur = sweep;
			lenght_cur = max_cur - min_cur + 1;
		}
		if((sweep == histogram_size - 2) && (fail_count_histogram[sweep] == 0) && (fail_count_histogram[sweep + 1] == 0))
    {
			max_cur = sweep + 1;
			lenght_cur = max_cur - min_cur + 1;
		}
		if(lenght_cur > lenght_max)
    {
			lenght_max = lenght_cur;
			*min = min_cur;
			*max = max_cur;
		}
	}

}


void clear_histogram(void)
{
  uint8
    dq,
    delay_step;

	for(dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
      {
        for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
        {
          fail_count_histogram_perbyte[dq][delay_step] = 0;
        }
      }
    }

uint32 DDRSS_dq_rd_wr_training_histogram_update(SDRAM_INTERFACE  interface, uint8 delay_step, uint32 read[DQ_TRAINING_PATTERN_SIZE], uint32 expected[DQ_TRAINING_PATTERN_SIZE], uint8 fail_count_histogram_perbyte[PHY_DQ_MAX_NUM][FAIL_HISTOGRAM_SIZE],uint8 dq)
{
 uint8 pattern_size;
 
	for(pattern_size=0;pattern_size<DQ_TRAINING_PATTERN_SIZE;pattern_size++){
		if(read[pattern_size]!=expected[pattern_size]) break;
	}
	
	if(pattern_size==DQ_TRAINING_PATTERN_SIZE) {
		return ALL_DQ_PASS_PAT;
	}
	else {
		fail_count_histogram_perbyte[dq][delay_step]++;
		return ALL_DQ_FAIL_PAT;
	} 
}

boolean DDRSS_EBI1_PHY_Set_Training_Data(SDRAM_INTERFACE interface, union ddr_trained_params tr_data)
{
	uint32 i=0;
	
	for(i=0;i<PHY_DQ_MAX_NUM;i++) 
	{
		HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(0, interface, tr_data.lpddr3_training.phy_cdc_params.dq_write_training[i], i);
	}
		for(i=0;i<PHY_DQ_MAX_NUM;i++) 
	{
		HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(0, interface, tr_data.lpddr3_training.phy_cdc_params.dq_read_training[i], i);
	}
		

	HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(0,interface,tr_data.lpddr3_training.phy_cdc_params.ca_training);
    
	return TRUE;
}
