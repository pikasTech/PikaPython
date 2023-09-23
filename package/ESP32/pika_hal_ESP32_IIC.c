#include "../PikaStdDevice/pika_hal.h"
#include "driver/i2c.h"

#define TEMP_SDA_IO 41 // 这里应该备多组i2c的io以对应不同I2Cx
#define TEMP_SCL_IO 42

typedef struct
{
    i2c_config_t conf;
    int port;
    unsigned int timeout;
    PIKA_HAL_IIC_MEM_ADDR mem_addr;
    PIKA_HAL_IIC_MEM_ADDR_ENA mem_addr_ena;
    PIKA_HAL_IIC_MEM_ADDR_SIZE mem_addr_size;
    unsigned int slave_addr;
} esp_iic_t;

int pika_hal_platform_IIC_open(pika_dev *dev, char *name)
{
    esp_iic_t *platform_i2c = pikaMalloc(sizeof(esp_iic_t));
    memset(platform_i2c, 0, sizeof(esp_iic_t));
    dev->platform_data = platform_i2c;
    int iic_num = fast_atoi(name + 3);

    if (!(name[0] == 'I' && name[1] == 'I' && name[2] == 'C') || iic_num <= I2C_NUM_MAX)
    {
        __platform_printf("IIC: Open %s failed\r\n", name);
        goto err;
    }

    platform_i2c->port = iic_num;

    return 0;
err:
    return -1;
}

int pika_hal_platform_IIC_close(pika_dev *dev)
{
    esp_iic_t *platform_i2c = (esp_iic_t *)dev->platform_data;
    if (NULL == platform_i2c)
    {
        goto err;
    }
    pikaFree(platform_i2c, sizeof(esp_iic_t));

    return 0;
err:
    return -1;
}

int pika_hal_platform_IIC_ioctl_config(pika_dev *dev,
                                       pika_hal_IIC_config *cfg)
{
    esp_iic_t *platform_i2c = (esp_iic_t *)dev->platform_data;
    if (dev->is_enabled)
    {
        goto err;
    }
    platform_i2c->timeout = cfg->timeout;
    platform_i2c->mem_addr = cfg->mem_addr;
    platform_i2c->mem_addr_ena = cfg->mem_addr_ena;
    platform_i2c->mem_addr_size = cfg->mem_addr_size;
    platform_i2c->slave_addr = cfg->slave_addr;
    platform_i2c->conf.sda_io_num = TEMP_SDA_IO;
    platform_i2c->conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    platform_i2c->conf.scl_io_num = TEMP_SCL_IO;
    platform_i2c->conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    switch (cfg->master_or_slave)
    {
    case PIKA_HAL_IIC_MASTER:
        platform_i2c->conf.mode = I2C_MODE_MASTER;
        platform_i2c->conf.master.clk_speed = cfg->speed;
        break;
    case PIKA_HAL_IIC_SLAVE:
        platform_i2c->conf.mode = I2C_MODE_SLAVE;
        if (cfg->address_width == PIKA_HAL_IIC_ADDRESS_WIDTH_7BIT)
        {
            platform_i2c->conf.slave.addr_10bit_en = 0;
        }
        else if (cfg->address_width == PIKA_HAL_IIC_ADDRESS_WIDTH_10BIT)
        {
            platform_i2c->conf.slave.addr_10bit_en = 1;
        }
        platform_i2c->conf.slave.maximum_speed = cfg->speed;
        break;
    default:
        goto err;
    }

    return 0;
err:
    return -1;
}

int pika_hal_platform_IIC_ioctl_enable(pika_dev *dev)
{
    esp_iic_t *platform_i2c = (esp_iic_t *)dev->platform_data;
    if (dev->is_enabled)
    {
        goto err;
    }
    i2c_config_t conf = platform_i2c->conf;
    esp_err_t res = i2c_param_config(platform_i2c->port, &conf);
    if (res != ESP_OK)
    {
        goto err;
    }
    i2c_driver_install(platform_i2c->port, conf.mode, 0, 0, 0);
    return 0;
err:
    return -1;
}

int pika_hal_platform_IIC_ioctl_disable(pika_dev *dev)
{
    if (!dev->is_enabled)
    {
        goto err;
    }
    esp_iic_t *platform_i2c = (esp_iic_t *)dev->platform_data;
    i2c_driver_delete(platform_i2c->port);

    return 0;
err:
    return -1;
}

int pika_hal_platform_IIC_write(pika_dev *dev, void *buf, size_t count)
{
    esp_iic_t *platform_i2c = (esp_iic_t *)dev->platform_data;
    int res = ESP_OK;
    if (platform_i2c->conf.mode == I2C_MODE_MASTER)
    {
        uint8_t buffer[360] = {0};

        i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
        assert(handle != NULL);

        res = i2c_master_start(handle);
        if (res != ESP_OK)
        {
            goto end;
        }

        res = i2c_master_write_byte(handle, platform_i2c->slave_addr << 1 | I2C_MASTER_WRITE, true);
        if (res != ESP_OK)
        {
            goto end;
        }

        if (platform_i2c->mem_addr_ena)
        {
            if (platform_i2c->mem_addr_size == PIKA_HAL_IIC_MEM_ADDR_SIZE_8BIT)
            {
                res = i2c_master_write(handle, (uint8_t *)&platform_i2c->mem_addr, 1, true);
            }
            else if (platform_i2c->mem_addr_size == PIKA_HAL_IIC_MEM_ADDR_SIZE_16BIT)
            {
                res = i2c_master_write(handle, (uint8_t *)&platform_i2c->mem_addr, 2, true);
            }

            if (res != ESP_OK)
            {
                goto end;
            }
        }

        res = i2c_master_write(handle, (uint8_t*)buf, count, true);
        if (res != ESP_OK)
        {
            goto end;
        }

        i2c_master_stop(handle);
        res = i2c_master_cmd_begin(platform_i2c->port, handle, platform_i2c->timeout / portTICK_PERIOD_MS);
    end:
        i2c_cmd_link_delete_static(handle);
        if (res != ESP_OK)
        {
            goto err;
        }
    }
    else if (platform_i2c->conf.mode == I2C_MODE_SLAVE)
    {
        res = i2c_slave_write_buffer(platform_i2c->port, (uint8_t*)buf, count, platform_i2c->timeout / portTICK_PERIOD_MS);
        if (res != ESP_OK)
        {
            goto err;
        }
    }

    return 0;
err:
    return -1;
}

int pika_hal_platform_IIC_read(pika_dev *dev, void *buf, size_t count)
{
    esp_iic_t *platform_i2c = (esp_iic_t *)dev->platform_data;
    int res = ESP_OK;
    if (platform_i2c->conf.mode == I2C_MODE_MASTER)
    {
        uint8_t buffer[360] = {0}; // 360是底层宏运算出来的结果，原宏为sizeof(i2c_cmd_desc_t) + sizeof(i2c_cmd_link_t) * 8，在components/driver/i2c.c中

        i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
        assert(handle != NULL);

        res = i2c_master_start(handle);
        if (res != ESP_OK)
        {
            goto end;
        }

        if (platform_i2c->mem_addr_ena)
        {
            res = i2c_master_write_byte(handle, platform_i2c->slave_addr << 1 | I2C_MASTER_WRITE, true);
            if (res != ESP_OK)
            {
                goto end;
            }
            if (platform_i2c->mem_addr_size == PIKA_HAL_IIC_MEM_ADDR_SIZE_8BIT)
            {
                res = i2c_master_write(handle, (uint8_t *)&platform_i2c->mem_addr, 1, true);
                if (res != ESP_OK)
                {
                    goto end;
                }
            }
            else if (platform_i2c->mem_addr_size == PIKA_HAL_IIC_MEM_ADDR_SIZE_16BIT)
            {
                res = i2c_master_write(handle, (uint8_t *)&platform_i2c->mem_addr, 2, true);
                if (res != ESP_OK)
                {
                    goto end;
                }
            }
            res = i2c_master_start(handle);
            if (res != ESP_OK)
            {
                goto end;
            }
        }

        res = i2c_master_write_byte(handle, platform_i2c->slave_addr << 1 | I2C_MASTER_READ, true);
        if (res != ESP_OK)
        {
            goto end;
        }

        res = i2c_master_read(handle, (uint8_t*)buf, count, I2C_MASTER_LAST_NACK);
        if (res != ESP_OK)
        {
            goto end;
        }

        i2c_master_stop(handle);
        res = i2c_master_cmd_begin(platform_i2c->port, handle, platform_i2c->timeout);

    end:
        i2c_cmd_link_delete_static(handle);
        if (res != ESP_OK)
        {
            goto err;
        }
    }
    else if (platform_i2c->conf.mode == I2C_MODE_SLAVE)
    {
        res = i2c_slave_read_buffer(platform_i2c->port, (uint8_t*)buf, count, platform_i2c->timeout / portTICK_PERIOD_MS);
        if (res != ESP_OK)
        {
            goto err;
        }
    }

    return 0;
err:
    return -1;
}
