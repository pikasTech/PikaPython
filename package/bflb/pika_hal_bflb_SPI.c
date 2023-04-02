#include "pika_hal_bflb_common.h"
#include "bflb_spi.h"
#include "bflb_dma.h"
#include "board.h"

typedef struct platform_data_SPI {
    struct bflb_device_s *spi;
    struct bflb_device_s *dma_tx;
    struct bflb_device_s *dma_rx;
} platform_data_SPI;

static int _num2spi(const char* name, struct bflb_device_s **spi) {
    if (name[0] == 's' && name[1] == 'p' && name[2] == 'i') {
        int spi_num = fast_atoi(name + 3);
        switch (spi_num) {
            case 0:
                *spi = bflb_device_get_by_name("spi0");
                break;
            default:
                return -1;
        }
        return 0;
    }
    return -1;
}

int pika_hal_platform_SPI_open(pika_dev* dev, char* name) {
    platform_data_SPI *data = pikaMalloc(sizeof(platform_data_SPI));
    memset(data, 0, sizeof(platform_data_SPI));
    dev->platform_data = data;
    if (_num2spi(name, &data->spi) != 0) {
        pikaFree(data, sizeof(platform_data_SPI));
        return -1;
    }
    return 0;
}

int pika_hal_platform_SPI_close(pika_dev* dev) {
    platform_data_SPI *data = (platform_data_SPI *)dev->platform_data;
    if (NULL != data) {
        pikaFree(data, sizeof(platform_data_SPI));
    }
    return 0;
}

int pika_hal_platform_SPI_ioctl_config(pika_dev* dev, pika_hal_SPI_config* cfg) {
    platform_data_SPI *data = (platform_data_SPI *)dev->platform_data;
    struct bflb_spi_config_s spi_cfg = {
        .freq = cfg->speed,
        .role = cfg->master_or_slave,
        .mode = cfg->mode,
        .data_width = SPI_DATA_WIDTH_8BIT, // Assume 8-bit data width
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
    bflb_spi_init(data->spi, &spi_cfg);
    return 0;
}

int pika_hal_platform_SPI_ioctl_enable(pika_dev* dev) {
    // Enable SPI here, if necessary
    return 0;
}

int pika_hal_platform_SPI_ioctl_disable(pika_dev* dev) {
    // Disable SPI here, if necessary
    return 0;
}

int pika_hal_platform_SPI_read(pika_dev* dev, void* buf, size_t count) {
    platform_data_SPI *data = (platform_data_SPI *)dev->platform_data;
    void *txbuff = malloc(count);
    int ret = bflb_spi_poll_exchange(data->spi, txbuff, buf, count);
    free(txbuff);
    return ret;
}

int pika_hal_platform_SPI_write(pika_dev* dev, void* buf, size_t count) {
    platform_data_SPI *data = (platform_data_SPI *)dev->platform_data;
    void *rxbuff = malloc(count);
    int ret = bflb_spi_poll_exchange(data->spi, buf, rxbuff, count);
    free(rxbuff);
    return ret;
}
