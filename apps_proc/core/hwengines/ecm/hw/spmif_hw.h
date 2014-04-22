#ifndef   __SPMIF_HW_H__
#define  __SPMIF_HW_H__

enum spmif_hw_opmode
{
    SPMIF_HW_STREAMING = 0,
	SPMIF_HW_ONESHOT = 1,
};
void spmif_hw_init(uint32 channel);
void spmif_hw_start(enum spmif_hw_opmode mode);
void spmif_hw_stop(void);
int spmif_hw_idle(void);
void spmif_hw_set_eot_length(uint32 eot);
void spmif_hw_set_eot_en(uint32 en);
void spmif_hw_set_pollgap_time(uint32 time);
void spmif_hw_set_readgap_time(uint32 time);
#endif

