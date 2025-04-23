/*
 * Copyright (c) 2024 RCSN
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "board.h"
#include "hpm_spi_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "DAP_config.h"
#include "DAP.h"

#define SWD_SPI_BASE               BOARD_APP_SPI_BASE
#define SWD_SPI_BASE_CLOCK_NAME    BOARD_APP_SPI_CLK_NAME
#define SWD_SPI_SCLK_FREQ          (50000000UL)


static void swd_emulation_init(void);


void PORT_SWD_SETUP(void)
{
    board_init_spi_pins(SWD_SPI_BASE);
    swd_emulation_init();
}

// Generate SWJ Sequence
//   count:  sequence bit count
//   data:   pointer to sequence bit data
//   return: none
#if ((DAP_SWD != 0) || (DAP_JTAG != 0))
void SWJ_Sequence (uint32_t count, const uint8_t *data)
{
    uint32_t n;
    uint32_t integer_val = (count / 8);
    uint32_t remaind_val = (count % 8);
    //printf("SWJ_Sequence %d\n", count);
    spi_set_transfer_mode(SWD_SPI_BASE, spi_trans_write_only);
    spi_set_data_bits(SWD_SPI_BASE, 8);
    SWD_SPI_BASE->CTRL |= SPI_CTRL_RXFIFORST_MASK | SPI_CTRL_TXFIFORST_MASK;
    while (SWD_SPI_BASE->STATUS & (SPI_CTRL_RXFIFORST_MASK | SPI_CTRL_TXFIFORST_MASK)) {
    };
    spi_set_write_data_count(SWD_SPI_BASE, integer_val);
    SWD_SPI_BASE->CMD = 0xFF;
    for (n = 0; n < integer_val; n++) {
        SWD_SPI_BASE->DATA = *(data + n);
    }
    while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
    };
    if (remaind_val > 0) {
        spi_set_write_data_count(SWD_SPI_BASE, 1);
        spi_set_data_bits(SWD_SPI_BASE, remaind_val);
        SWD_SPI_BASE->DATA = *(data + integer_val);
        SWD_SPI_BASE->CMD = 0xFF;
        while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
        };    
    }
}
#endif

// Generate SWD Sequence
//   info:   sequence information
//   swdo:   pointer to SWDIO generated data
//   swdi:   pointer to SWDIO captured data
//   return: none
#if (DAP_SWD != 0)
void SWD_Sequence (uint32_t info, const uint8_t *swdo, uint8_t *swdi)
{
    uint32_t count, integer_val, remaind_val, n = 0;

    count = info & SWD_SEQUENCE_CLK;
    if (count == 0U) {
        count = 64U;
    }
    //printf("SWD_Sequence\n");
    SWD_SPI_BASE->CTRL |= SPI_CTRL_RXFIFORST_MASK | SPI_CTRL_TXFIFORST_MASK;
    while (SWD_SPI_BASE->STATUS & (SPI_CTRL_RXFIFORST_MASK | SPI_CTRL_TXFIFORST_MASK)) {
    };
    integer_val = (count / 8);
    remaind_val = (count % 8);
    spi_set_data_bits(SWD_SPI_BASE, 8);
    if (info & SWD_SEQUENCE_DIN) {
        spi_set_read_data_count(SWD_SPI_BASE, integer_val);
        spi_set_transfer_mode(SWD_SPI_BASE, spi_trans_read_only);
        SWD_SPI_BASE->CMD = 0xFF;
        while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
        };
        while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
            *(swdi + n) = SWD_SPI_BASE->DATA;
            n++;
        }
        while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
        };
        if (remaind_val > 0) {
            spi_set_read_data_count(SWD_SPI_BASE, 1);
            spi_set_data_bits(SWD_SPI_BASE, remaind_val);
            SWD_SPI_BASE->CMD = 0xFF;
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
            };
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
               *(swdi + integer_val) = SWD_SPI_BASE->DATA ;
            }
        }
    } else {
        spi_set_write_data_count(SWD_SPI_BASE, integer_val);
        spi_set_transfer_mode(SWD_SPI_BASE, spi_trans_write_only);
        SWD_SPI_BASE->CMD = 0xFF;
        for (n = 0; n < integer_val; n++) {
            SWD_SPI_BASE->DATA = *(swdo + n);
        }
        while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
        };
        if (remaind_val > 0) {
            spi_set_data_bits(SWD_SPI_BASE, remaind_val);
            SWD_SPI_BASE->DATA = *(swdo + integer_val);
            SWD_SPI_BASE->CMD = 0xFF;
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
        }
    }

}

// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]
uint8_t  SWD_Transfer(uint32_t request, uint32_t *data)
{
    uint8_t ack = 0;
    uint32_t parity = 0;
    uint8_t calc_parity = 0;
    uint8_t i = 0;
    uint32_t host_data = 0x81; /* start = 1, stop = 0, park = 1 0b10000001*/
    uint32_t dummy = 0;
    uint8_t ack_width = 0;
    SWD_SPI_BASE->CTRL |= SPI_CTRL_RXFIFORST_MASK | SPI_CTRL_TXFIFORST_MASK;
    spi_set_write_data_count(SWD_SPI_BASE, 1);
    spi_set_read_data_count(SWD_SPI_BASE, 1);
    while (SWD_SPI_BASE->STATUS & (SPI_CTRL_RXFIFORST_MASK | SPI_CTRL_TXFIFORST_MASK)) {
    };
    switch (request & 0x0F)
    {
    case 0:
    case 3:
    case 5:
    case 6:
    case 9:
    case 10:
    case 12:
    case 15:
        parity = 0;
        break;
    default:
        parity = 1;
        break;
    }
    host_data |= ((request & 0x0F) << 1) | (parity << 5);
    parity = 0;
    spi_set_transfer_mode(SWD_SPI_BASE, spi_trans_write_only);
    if (request & DAP_TRANSFER_RnW) {
        spi_set_data_bits(SWD_SPI_BASE, 8);
        ack_width = 4;
    } else {
        for (i = 0; i < 32; i++) {
            parity += (((*data) >> i) & 0x01);
        }
        spi_set_data_bits(SWD_SPI_BASE, 8 + DAP_Data.swd_conf.turnaround);
        if (DAP_Data.swd_conf.turnaround > 0) {
            host_data <<= 1;
        }
        ack_width = 5;
    }
    SWD_SPI_BASE->CMD = 0xFF;
    for (uint8_t i = 0; i < DAP_Data.swd_conf.turnaround; i++) {
        SWD_SPI_BASE->DATA = host_data;
    }
    while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
    };
    spi_set_transfer_mode(SWD_SPI_BASE, spi_trans_read_only);
    spi_set_data_bits(SWD_SPI_BASE, ack_width);
    SWD_SPI_BASE->CMD = 0xFF;
    while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
    };
    while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
        ack = SWD_SPI_BASE->DATA;
    }
    while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
    };
    ack >>= 1;
    if (ack == DAP_TRANSFER_OK) { /* OK response */
        /* Data transfer */
        if (request & DAP_TRANSFER_RnW) {
            /* Read data */
            SWD_SPI_BASE->TRANSCTRL = 0x2000000; /* only read mode*/
            SWD_SPI_BASE->TRANSFMT = 0x1F18; /* datalen = 32bit, mosibidir = 1, lsb=1 */
            SWD_SPI_BASE->CMD = 0xFF;
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
            };
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
                dummy = SWD_SPI_BASE->DATA;
            }
            spi_set_data_bits(SWD_SPI_BASE, 1 + DAP_Data.swd_conf.turnaround);
            SWD_SPI_BASE->CMD = 0xFF;
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
            };
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
                parity = (SWD_SPI_BASE->DATA) & 0x01;
            }
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
            for (i = 0; i < 32U; i++) {
                calc_parity += ((dummy >> i) & 0x01);
            }
            if (parity != (calc_parity & 0x01)) {
                ack = DAP_TRANSFER_ERROR;
            }
            if (data != NULL) {
                (*data) = dummy;
            }
        } else {
            SWD_SPI_BASE->TRANSCTRL = 0x1000000; /* only write mode*/
            SWD_SPI_BASE->TRANSFMT = 0x1F18; /* datalen = 32bit, mosibidir = 1, lsb=1 */
            SWD_SPI_BASE->CMD = 0xFF;
            SWD_SPI_BASE->DATA = (*data);
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
            spi_set_data_bits(SWD_SPI_BASE, 1 + DAP_Data.transfer.idle_cycles);
            SWD_SPI_BASE->CMD = 0xFF;
            SWD_SPI_BASE->DATA = parity;
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
            /* Capture Timestamp */
            if (request & DAP_TRANSFER_TIMESTAMP) {
                DAP_Data.timestamp = TIMESTAMP_GET();
            }
        }
        return ack;
    }

    /* WAIT or FAULT response */
    if ((ack == DAP_TRANSFER_WAIT) || (ack == DAP_TRANSFER_FAULT)) {
        SWD_SPI_BASE->TRANSCTRL = 0x02000000; /* only read mode*/
        SWD_SPI_BASE->TRANSFMT = 0x0018; /* datalen = 1bit, mosibidir = 1, lsb=1 */
        /* Dummy Read RDATA[0:31] + Parity */
        if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) != 0U)) {
            spi_set_write_data_count(SWD_SPI_BASE, 33);
            SWD_SPI_BASE->CMD = 0xFF;
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
            };
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
                parity = (SWD_SPI_BASE->DATA) & 0x01;
            }
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
        }
        /* Turnaround */
        if (DAP_Data.swd_conf.turnaround > 0) {
            spi_set_write_data_count(SWD_SPI_BASE, DAP_Data.swd_conf.turnaround);
            SWD_SPI_BASE->CMD = 0xFF;
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) == SPI_STATUS_RXEMPTY_MASK) {
            };
            while ((SWD_SPI_BASE->STATUS & SPI_STATUS_RXEMPTY_MASK) != SPI_STATUS_RXEMPTY_MASK) {
                parity = (SWD_SPI_BASE->DATA) & 0x01;
            }
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
        }
        /* Dummy Write WDATA[0:31] + Parity */
        if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) == 0U)) {
            SWD_SPI_BASE->TRANSCTRL = 0x01000000; /* only write mode*/
            SWD_SPI_BASE->TRANSFMT = 0x0018; /* datalen = 1bit, mosibidir = 1, lsb=1 */
            spi_set_write_data_count(SWD_SPI_BASE, 33);
            SWD_SPI_BASE->CMD = 0xFF;
            for (i = 0; i < 33; i++) {
                SWD_SPI_BASE->DATA = 0;
                while ((SWD_SPI_BASE->STATUS & SPI_STATUS_TXFULL_MASK) == SPI_STATUS_TXFULL_MASK) {
                };
            }
            while (SWD_SPI_BASE->STATUS & SPI_STATUS_SPIACTIVE_MASK) {
            };
        }
        return ack;
    }
    /* Protocol error */
    spi_set_transfer_mode(SWD_SPI_BASE, spi_trans_write_only);
    spi_set_write_data_count(SWD_SPI_BASE, DAP_Data.swd_conf.turnaround + 32U + 1U);
    SWD_SPI_BASE->CMD = 0xFF;
    for (i = 0; i < DAP_Data.swd_conf.turnaround + 32U + 1U; i++) {
        SWD_SPI_BASE->DATA = 0;
        while ((SWD_SPI_BASE->STATUS & SPI_STATUS_TXFULL_MASK) == SPI_STATUS_TXFULL_MASK) {
        };
    }
    spi_set_data_bits(SWD_SPI_BASE, 1);
    return ack;
}
#endif

static void swd_emulation_init(void)
{
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    uint32_t spi_clcok;
    uint32_t pll_clk = 0, div = 0;
    /* set SPI sclk frequency for master */
    spi_clcok = board_init_spi_clock(SWD_SPI_BASE);
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.cs2sclk = spi_cs2sclk_half_sclk_1;
    timing_config.master_config.csht = spi_csht_half_sclk_1;
    timing_config.master_config.clk_src_freq_in_hz = spi_clcok;
    timing_config.master_config.sclk_freq_in_hz = SWD_SPI_SCLK_FREQ;
    if (status_success != spi_master_timing_init(SWD_SPI_BASE, &timing_config)) {
        spi_master_set_sclk_div(SWD_SPI_BASE, 0xFF);
        pll_clk = get_frequency_for_source(clock_source_pll1_clk0);
        div = pll_clk / SWD_SPI_SCLK_FREQ;
        clock_set_source_divider(SWD_SPI_BASE_CLOCK_NAME, clk_src_pll1_clk0, div);
    }

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.master_config.addr_len_in_bytes = 1U;
    format_config.common_config.data_len_in_bits = 1;
    format_config.common_config.data_merge = false;
    format_config.common_config.mosi_bidir = true;
    format_config.common_config.lsb = true;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_low_idle;
    format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges;
    spi_format_init(SWD_SPI_BASE, &format_config);

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;
    control_config.master_config.addr_enable = false;
    control_config.master_config.addr_phase_fmt = spi_address_phase_format_single_io_mode;
    control_config.common_config.trans_mode = spi_trans_write_dummy_read;
    control_config.common_config.data_phase_fmt = spi_single_io_mode;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;
    spi_control_init(SWD_SPI_BASE, &control_config, 1, 1);
    //  SWD_SPI_BASE->CTRL |= SPI_CTRL_TXDMAEN_MASK | SPI_CTRL_RXDMAEN_MASK;
}


