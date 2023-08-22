#include "../PikaStdDevice/pika_hal.h"
#include <rtthread.h>
#include <rtdevice.h>

typedef struct platform_data_GPIO {
    uint32_t pin_num;
} platform_data_GPIO;

int pika_hal_platform_GPIO_open(pika_dev* dev, char* name) {
	  rt_kprintf("\r\n=%s==%s=%d=name:%s==\r\n",__FILE__,__FUNCTION__,__LINE__,name);
    __platform_printf("Open: %s \r\n", name);
    platform_data_GPIO* data = pikaMalloc(sizeof(platform_data_GPIO));
#ifdef RT_USING_PIN
    data->pin_num =  rt_pin_get(name) ;
#endif
    dev->platform_data = data;
    return 0;
}

int pika_hal_platform_GPIO_close(pika_dev* dev) {
	  rt_kprintf("\r\n=%s==%s=%d===\r\n",__FILE__,__FUNCTION__,__LINE__);
    if (NULL != dev->platform_data) {
        pikaFree(dev->platform_data, sizeof(platform_data_GPIO));
        dev->platform_data = NULL;
    }
    return 0;
}

int pika_hal_platform_GPIO_read(pika_dev* dev, void* buf, size_t count) {

    platform_data_GPIO* data = dev->platform_data;
	  uint32_t level;
	  rt_kprintf("\r\n=%s==%s=%d=gpio:%d==\r\n",__FILE__,__FUNCTION__,__LINE__,data->pin_num);
#ifdef RT_USING_PIN
    level = rt_pin_read(data->pin_num);
#endif
    if (level != 1 && level != 0) {
        return -1;
    }
    memcpy(buf, &level, count);
    return 0;
}

int pika_hal_platform_GPIO_write(pika_dev* dev, void* buf, size_t count) {


    platform_data_GPIO* data = dev->platform_data;

    uint32_t level = 0;
    memcpy(&level, buf, count);
#ifdef RT_USING_PIN	
    if (level == 0) {
		    rt_pin_write(data->pin_num, PIN_LOW);
        return 0;
    }
    if (level == 1) {
				rt_pin_write(data->pin_num, PIN_HIGH);
        return 0;
    }
#endif
    return 0;
}

int pika_hal_platform_GPIO_ioctl_enable(pika_dev* dev) {
	
		platform_data_GPIO* data = dev->platform_data;

		rt_kprintf("\r\n=%s==%s=%d=pin_num:%x==\r\n",__FILE__,__FUNCTION__,__LINE__,data->pin_num);
	  
	 /* TODO */
    return 0;
}

int pika_hal_platform_GPIO_ioctl_disable(pika_dev* dev) {
	  rt_kprintf("\r\n=%s==%s=%d===\r\n",__FILE__,__FUNCTION__,__LINE__);
    platform_data_GPIO* data = dev->platform_data;
    return -1;
}

int pika_hal_platform_GPIO_ioctl_config(pika_dev* dev,
                                        pika_hal_GPIO_config* cfg) {
																				
	  rt_kprintf("\r\n=%s==%s=%d=dir:%d==\r\n",__FILE__,__FUNCTION__,__LINE__,cfg->dir);

    platform_data_GPIO* data = dev->platform_data;
		uint8_t pinMode = 0;
																					
    switch (cfg->dir) {
        case PIKA_HAL_GPIO_DIR_IN:
					  switch(cfg->pull)
						{
								case PIKA_HAL_GPIO_PULL_UP:
					          pinMode = PIN_MODE_INPUT_PULLUP;
										break;
								case PIKA_HAL_GPIO_PULL_DOWN:
					          pinMode = PIN_MODE_INPUT_PULLDOWN;
										break;
								default:
										pinMode = PIN_MODE_INPUT;
						}
            break;
        case PIKA_HAL_GPIO_DIR_OUT:
					  pinMode = PIN_MODE_OUTPUT;
            break;
        default:
            pinMode = PIN_MODE_OUTPUT;
    }
#ifdef RT_USING_PIN	
    rt_pin_mode(data->pin_num, pinMode);
#endif
    return 0;
}
