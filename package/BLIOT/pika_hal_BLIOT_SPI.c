#include <hosal_dma.h>
#include <hosal_spi.h>
#include "../PikaStdDevice/pika_hal.h"

static int _num2pin(int num, uint8_t* mosi, uint8_t* miso, uint8_t* clk) {
    /***********************   BL602  **************************
     *    SPI0  ----->     MOSI:P20, MISO:P21, SCLK:P3
     */
    switch (num) {
        case 0:
            *mosi = 20;
            *miso = 21;
            *clk = 3;
            break;
        default:
            return -1;
    }
#if PIKA_DEBUG_ENABLE
    __platform_printf("SPI%d: mosi:%d, miso:%d, scli:%d\r\n", num, *mosi, *miso,
                      *clk);
#endif
    return 0;
}

int pika_hal_platform_SPI_open(pika_dev* dev, char* name) {
    hosal_spi_dev_t* platform_spi = pikaMalloc(sizeof(hosal_spi_dev_t));
    memset(platform_spi, 0, sizeof(hosal_spi_dev_t));
    dev->platform_data = platform_spi;
    /* Support SPI0, SPI1, SPI2 */
    if (name[0] == 'S' && name[1] == 'P' && name[2] == 'I') {
        int spi_num = fast_atoi(name + 3);
        platform_spi->port = 0;
        if (0 == _num2pin(spi_num, &platform_spi->config.pin_mosi,
                          &platform_spi->config.pin_miso,
                          &platform_spi->config.pin_clk)) {
#if PIKA_DEBUG_ENABLE
            __platform_printf("SPI: mosi:%d, miso:%d, scli:%d\r\n",
                              platform_spi->config.pin_mosi,
                              platform_spi->config.pin_miso,
                              platform_spi->config.pin_clk);
#endif
            return 0;
        } else {
            __platform_printf("SPI: Open SPI%d failed\r\n", spi_num);
            return -1;
        }
    }
    return -1;
}

int pika_hal_platform_SPI_close(pika_dev* dev) {
    hosal_spi_dev_t* platform_spi = (hosal_spi_dev_t*)dev->platform_data;
    if (NULL != platform_spi) {
        pikaFree(platform_spi, sizeof(hosal_spi_dev_t));
    }
    return 0;
}

int pika_hal_platform_SPI_ioctl_config(pika_dev* dev,
                                       pika_hal_SPI_config* cfg) {
    hosal_spi_dev_t* platform_spi = (hosal_spi_dev_t*)dev->platform_data;
    if (!dev->is_enabled) {
        platform_spi->port = 0;
        platform_spi->config.dma_enable = 1;
        hosal_dma_init();
        platform_spi->config.freq = cfg->speed;
        platform_spi->p_arg = NULL;
        switch (cfg->master_or_slave) {
            case PIKA_HAL_SPI_MASTER:
                platform_spi->config.mode = HOSAL_SPI_MODE_MASTER;
                break;
            case PIKA_HAL_SPI_SLAVE:
                platform_spi->config.mode = HOSAL_SPI_MODE_SLAVE;
                break;
            default:
                return -1;
        }
        switch (cfg->mode) {
            case PIKA_HAL_SPI_MODE_0:
                platform_spi->config.polar_phase = 0;
                break;
            case PIKA_HAL_SPI_MODE_1:
                platform_spi->config.polar_phase = 1;
                break;
            case PIKA_HAL_SPI_MODE_2:
                platform_spi->config.polar_phase = 2;
                break;
            case PIKA_HAL_SPI_MODE_3:
                platform_spi->config.polar_phase = 3;
                break;
            default:
                return -1;
        }
#if PIKA_DEBUG_ENABLE
        __platform_printf("SPI: freq:%d, mode:%d, polar_phase:%d\r\n",
                          platform_spi->config.freq, platform_spi->config.mode,
                          platform_spi->config.polar_phase);
#endif
    }
    return 0;
}

int pika_hal_platform_SPI_ioctl_enable(pika_dev* dev) {
    if (!dev->is_enabled) {
        hosal_spi_dev_t* platform_spi = (hosal_spi_dev_t*)dev->platform_data;
#if PIKA_DEBUG_ENABLE
        __platform_printf(
            "SPI: Open SPI%d, freq:%d, mode:%d, polar_phase:%d\r\n",
            platform_spi->port, platform_spi->config.freq,
            platform_spi->config.mode, platform_spi->config.polar_phase);
#endif
        if (0 != hosal_spi_init(platform_spi)) {
            __platform_printf("SPI: Open SPI%d failed\r\n", platform_spi->port);
            return -1;
        }
        return 0;
    }
    return -1;
}

int pika_hal_platform_SPI_ioctl_disable(pika_dev* dev) {
    if (dev->is_enabled) {
        hosal_spi_dev_t* platform_spi = (hosal_spi_dev_t*)dev->platform_data;
        hosal_spi_finalize(platform_spi);
        return 0;
    }
    return -1;
}

int pika_hal_platform_SPI_write(pika_dev* dev, void* buf, size_t count) {
    hosal_spi_dev_t* platform_spi = (hosal_spi_dev_t*)dev->platform_data;
    pika_hal_SPI_config* cfg = (pika_hal_SPI_config*)dev->ioctl_config;
#if PIKA_DEBUG_ENABLE
    __platform_printf("SPI: Write %d bytes\r\n", count);
#endif
    int ret = hosal_spi_send(platform_spi, buf, count, cfg->timeout);
    if (0 != ret) {
        __platform_printf("SPI: Write %d bytes failed\r\n", count);
    }
    return ret;
}

int pika_hal_platform_SPI_read(pika_dev* dev, void* buf, size_t count) {
    hosal_spi_dev_t* platform_spi = (hosal_spi_dev_t*)dev->platform_data;
    pika_hal_SPI_config* cfg = (pika_hal_SPI_config*)dev->ioctl_config;
#if PIKA_DEBUG_ENABLE
    __platform_printf("SPI: Read %d bytes\r\n", count);
#endif
    int ret = hosal_spi_recv(platform_spi, buf, count, cfg->timeout);
    if (0 != ret) {
        __platform_printf("SPI: Read %d bytes failed\r\n", count);
    }
    return ret;
}
