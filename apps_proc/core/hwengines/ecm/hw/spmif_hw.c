#include "msmhwiobase.h"
#include "HALhwio.h"
#include "spmif_hw.h"
#include "spmif_hwio.h"
#include "spmif_cfgdata.h"

void spmif_hw_init(uint32 channel)
{
    /* the channel address should be taken from HWIO
     * q = 5 and n = channel
     */
    uint32 channel_address = 0x02400000+0x01000*5+0x08000*channel;
    HWIO_OUTF(SPMI_FETCHER_CONFIG, PMIC_SLAVE_ID, SPMIF_PMIC_SLAVE_ID);
    HWIO_OUT(SPMI_FETCHER_POLLGAP_TIME, POLLGAP_TIME);
    HWIO_OUT(SPMI_FETCHER_READGAP_TIME, READGAP_TIME);
    HWIO_OUT(SPMI_FETCHER_ECMFIFO_DATA_CTRL, (ECMFIFO_DATA_ADDR & 0xFF));
    HWIO_OUT(SPMI_FETCHER_ECMFIFO_STATUS_CTRL, (ECMFIFO_STS_ADDR & 0xFF));
    HWIO_OUT(SPMI_FETCHER_PMICARB_CHNL_BASE, channel_address);
}
void spmif_hw_start(enum spmif_hw_opmode mode)
{
        HWIO_OUTF(SPMI_FETCHER_CONFIG, MODE, mode);
	 HWIO_OUTF(SPMI_FETCHER_CMD, ECM_START, 1);
}

void spmif_hw_set_eot_length(uint32 eot)
{
    HWIO_OUTF(SPMI_FETCHER_EOT_LENGTH_CTRL, EOT_LENGTH, eot);
    HWIO_OUTF(SPMI_FETCHER_EOT_LENGTH_CTRL, EOT_DISABLE, 0 );
}

void spmif_hw_set_eot_en(uint32 en)
{
    HWIO_OUTF(SPMI_FETCHER_EOT_LENGTH_CTRL, EOT_DISABLE, en&1 );
}


void spmif_hw_stop(void)
{
    if (HWIO_INF(SPMI_FETCHER_STATUS, BUSY)) {
		HWIO_OUTF(SPMI_FETCHER_CMD, ECM_STOP, 1);
	}
}

int spmif_hw_idle(void)
{
	return HWIO_INF(SPMI_FETCHER_STATUS, BUSY) == 0;
}

void spmif_hw_set_pollgap_time(uint32 time)
{
    HWIO_OUT(SPMI_FETCHER_POLLGAP_TIME, time);
}

void spmif_hw_set_readgap_time(uint32 time)
{
    HWIO_OUT(SPMI_FETCHER_READGAP_TIME, time);
}
