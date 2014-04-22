/* Provides the stub interface for querying clock information on the PC */



/* Get the current, min and max performance levels */
uint32 clk_regime_msm_get_perf_level
(
  uint32 clk,
  uint32             *cur_level,  /* Can be NULL if don't care */
  uint32             *min_level,  /* Can be NULL if don't care */
  uint32             *max_level   /* Can be NULL if don't care */
);

/* Get the frequency for the given performance level */
uint32 clk_regime_msm_get_perf_level_freq_hz
(
  uint32 clk,
  uint32              perf_level
);

