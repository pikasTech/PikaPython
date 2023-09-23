#include "../PikaStdDevice/pika_hal.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "pika_hal.h"

#define TEMP_MOSI_IO 11 // 这里应该备多组i2c的io以对应不同I2Cx
#define TEMP_MISO_IO -1
#define TEMP_SCLK_IO 12
#define TEMP_CS_IO 10
#define TEMP_QUEUE_SIZE 128 // 这里是开启dma传输时，用于队列传输的大小，通常大于spi一次性发送的数据，如果不使能DMA，则不用管
typedef struct
{
    spi_bus_config_t buscfg;
    PIKA_HAL_SPI_MASTER_OR_SLAVE master_or_slave;
    unsigned int time_out;
    union
    {
        spi_device_interface_config_t devcfg;
        spi_slave_interface_config_t slvcfg;
    };

    spi_device_handle_t devhandle;

    int host_id;

} esp_spi_t;

int pika_hal_platform_SPI_open(pika_dev *dev, char *name)
{
    esp_spi_t *platform_spi = pikaMalloc(sizeof(esp_spi_t));
    memset(platform_spi, 0, sizeof(esp_spi_t));
    dev->platform_data = platform_spi;

    int spi_num = fast_atoi(name + 3);
    if (!(name[0] == 'S' && name[1] == 'P' && name[2] == 'I') || spi_num >= SPI_HOST_MAX)
    {
        __platform_printf("SPI: Open %s failed\r\n", name);
        goto err;
    }

    platform_spi->host_id = spi_num - 1;

    return 0;
err:
    return -1;
}

int pika_hal_platform_SPI_close(pika_dev *dev)
{
    esp_spi_t *platform_spi = (esp_spi_t *)dev->platform_data;
    if (NULL == platform_spi)
    {
        goto err;
    }
    pikaFree(platform_spi, sizeof(esp_spi_t));

    return 0;
err:
    return -1;
}

int pika_hal_platform_SPI_ioctl_config(pika_dev *dev,
                                       pika_hal_SPI_config *cfg)
{
    if (dev->is_enabled)
    {
        goto err;
    }

    esp_spi_t *platform_spi = (esp_spi_t *)dev->platform_data;
    platform_spi->buscfg.mosi_io_num = TEMP_MOSI_IO;
    platform_spi->buscfg.miso_io_num = TEMP_MISO_IO;
    platform_spi->buscfg.sclk_io_num = TEMP_SCLK_IO;
    platform_spi->buscfg.quadwp_io_num = -1;
    platform_spi->buscfg.quadhd_io_num = -1;
    platform_spi->buscfg.max_transfer_sz = 4000,

    platform_spi->master_or_slave = cfg->master_or_slave;
    platform_spi->time_out = cfg->timeout;

    if (cfg->master_or_slave == PIKA_HAL_SPI_MASTER)
    {
        platform_spi->devcfg.clock_speed_hz = cfg->speed;
        // platform_spi->devcfg.mode = cfg->mode;
        switch(cfg->mode){
            case PIKA_HAL_SPI_MODE_0:
                platform_spi->devcfg.mode = 0;
                break;
            case PIKA_HAL_SPI_MODE_1:
                platform_spi->devcfg.mode = 1;
                break;
            case PIKA_HAL_SPI_MODE_2:
                platform_spi->devcfg.mode = 2;
                break;
            case PIKA_HAL_SPI_MODE_3:
                platform_spi->devcfg.mode = 3;
                break;
            default:
                platform_spi->devcfg.mode = 0;
                break;
        }
        platform_spi->devcfg.spics_io_num = TEMP_CS_IO;
        platform_spi->devcfg.queue_size = TEMP_QUEUE_SIZE;
    }
    else if (cfg->master_or_slave == PIKA_HAL_SPI_SLAVE)
    {
        // platform_spi->slvcfg.mode = cfg->mode;
        switch(cfg->mode){
            case PIKA_HAL_SPI_MODE_0:
                platform_spi->slvcfg.mode = 0;
                break;
            case PIKA_HAL_SPI_MODE_1:
                platform_spi->slvcfg.mode = 1;
                break;
            case PIKA_HAL_SPI_MODE_2:
                platform_spi->slvcfg.mode = 2;
                break;
            case PIKA_HAL_SPI_MODE_3:
                platform_spi->slvcfg.mode = 3;
                break;
            default:
                platform_spi->slvcfg.mode = 0;
                break;
        }
        platform_spi->slvcfg.spics_io_num = TEMP_CS_IO;
        platform_spi->slvcfg.queue_size = TEMP_QUEUE_SIZE;
        platform_spi->slvcfg.flags = 0;
        platform_spi->slvcfg.post_setup_cb = NULL;
        platform_spi->slvcfg.post_trans_cb = NULL;
    }
    else
    {
        goto err;
    }

    return 0;
err:
    return -1;
}

int pika_hal_platform_SPI_ioctl_enable(pika_dev *dev)
{
    if (dev->is_enabled)
    {
        goto err;
    }

    esp_spi_t *platform_spi = (esp_spi_t *)dev->platform_data;
    int ret = 0;
    if (platform_spi->master_or_slave == PIKA_HAL_SPI_MASTER)
    {
        ret = spi_bus_initialize(platform_spi->host_id, &platform_spi->buscfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK)
        {
            goto err;
        }
        ret = spi_bus_add_device(platform_spi->host_id, &platform_spi->devcfg, &platform_spi->devhandle);
        if (ret != ESP_OK)
        {
            goto err;
        }
    }
    else if (platform_spi->master_or_slave == PIKA_HAL_SPI_SLAVE)
    {
        ret = spi_slave_initialize(platform_spi->host_id, &platform_spi->buscfg, &platform_spi->slvcfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK)
        {
            goto err;
        }
    }

    return 0;
err:
    return -1;
}

int pika_hal_platform_SPI_ioctl_disable(pika_dev *dev)
{
    if (!dev->is_enabled)
    {
        goto err;
    }
 
    esp_spi_t *platform_spi = (esp_spi_t *)dev->platform_data;

    if (platform_spi->master_or_slave == PIKA_HAL_SPI_MASTER)
    {
        spi_bus_remove_device(platform_spi->devhandle);
        spi_bus_free(platform_spi->host_id);
    }
    else if (platform_spi->master_or_slave == PIKA_HAL_SPI_SLAVE)
    {
        spi_slave_free(platform_spi->host_id);
    }

    return 0;
err:
    return -1;
}

int pika_hal_platform_SPI_write(pika_dev *dev, void *buf, size_t count)
{
    esp_spi_t *platform_spi = (esp_spi_t *)dev->platform_data;

    if (platform_spi->master_or_slave == PIKA_HAL_SPI_MASTER)
    {
        spi_transaction_t t = {0};
        t.length = 8 * count;
        t.tx_buffer = buf;
        // E (1388) spi_master: check_trans_valid(774): SPI_TRANS_USE_TXDATA only available for txdata transfer <= 32 bits
        // t.flags = SPI_TRANS_USE_TXDATA;
        esp_err_t ret = spi_device_polling_transmit(platform_spi->devhandle, &t);
        if (ret != ESP_OK)
        {
            goto err;
        }
    }
    else if (platform_spi->master_or_slave == PIKA_HAL_SPI_SLAVE)
    {
        spi_slave_transaction_t t;
        t.length = count * 8;
        t.tx_buffer = buf;
        t.rx_buffer = NULL;
        esp_err_t ret = spi_slave_transmit(platform_spi->host_id, &t, platform_spi->time_out);
        if (ret != ESP_OK)
        {
            goto err;
        }
    }

    return 0;
err:
    return -1;
}

int pika_hal_platform_SPI_read(pika_dev *dev, void *buf, size_t count)
{
    esp_spi_t *platform_spi = (esp_spi_t *)dev->platform_data;

    if (platform_spi->master_or_slave == PIKA_HAL_SPI_MASTER)
    {
        spi_device_acquire_bus(platform_spi->devhandle, portMAX_DELAY);
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 8 * count;
        t.flags = SPI_TRANS_USE_RXDATA;
        t.rx_buffer = buf;

        esp_err_t ret = spi_device_polling_transmit(platform_spi->devhandle, &t);
        spi_device_release_bus(platform_spi->devhandle);
        if (ret != ESP_OK)
        {
            goto err;
        }
    }
    else if (platform_spi->master_or_slave == PIKA_HAL_SPI_SLAVE)
    {
        static spi_slave_transaction_t t;
        t.length = count * 8;
        t.tx_buffer = NULL;
        t.rx_buffer = buf;
        esp_err_t ret = spi_slave_transmit(platform_spi->host_id, &t, platform_spi->time_out);
        if (ret != ESP_OK)
        {
            goto err;
        }
    }

    return 0;
err:
    return -1;
}
