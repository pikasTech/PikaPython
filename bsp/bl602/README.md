# How to Compile

1. Install Wine

    ```
    sudo apt install wine 
    ```

2. Install SDK:
    > https://blog.csdn.net/Boantong_/article/details/125604649?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166617877716800182741996%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=166617877716800182741996&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-3-125604649-null-null.nonecase&utm_term=wb2&spm=1018.2226.3001.4450

3. 

``` bash
mv pikascript_bl602 Ai-Thinker-Wb2
cd Ai-Thinker-Wb2/pikascript_bl602
sh make.sh
```

# How to Flash
> https://blog.csdn.net/Boantong_/article/details/125781602?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166617877716800182741996%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=166617877716800182741996&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-1-125781602-null-null.nonecase&utm_term=wb2&spm=1018.2226.3001.4450

# IIC

``` c
    pika_dev* iic = pika_hal_open(PIKA_HAL_IIC, "IIC3");
    pika_hal_IIC_config iic_cfg = {0};
    iic_cfg.slave_addr = 0x12;
    iic_cfg.mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE;
    iic_cfg.mem_addr = 0x00;
    iic_cfg.speed = 400000;
    uint32_t err = 0;
    pika_hal_ioctl(iic, PIKA_HAL_IOCTL_CONFIG, &iic_cfg);
    pika_hal_ioctl(iic, PIKA_HAL_IOCTL_ENABLE, NULL);
    uint8_t res = 0;
    err = pika_hal_read(iic, &res, 1);
    if (0 != err) {
        printf("read error, err=%d\r\n", err);
    }
    printf("IIC3 read reg=%02x, res=%02x\r\n", iic_cfg.mem_addr, res);
```

log:
```
======[pikascript packages installed]======
pikascript-core==v1.11.7 (2022/11/19 19:43:26)
PikaStdDevice==latest
PikaStdLib==v1.11.7
===========================================
hello pikascript
IIC: Open IIC3 on SCL:P12, SDA:P11
IIC: Config IIC0, speed:400000, address_width:0,master_or_slave: 1
IIC: Enable IIC0 on SCL:P12, SDA:P11
IIC: Read 1 bytes from 0x12
IIC: Read 1 bytes from 0x12, mem_addr:0x00
IIC3 read reg=00, res=e7
>>> [ Info] In PikaSciprt Demo...
======[pikascript packages installed]======
pikascript-core==v1.11.7 (2022/11/19 19:43:26)
PikaStdDevice==latest
PikaStdLib==v1.11.7
===========================================
hello pikascript
IIC: Open IIC3 on SCL:P12, SDA:P11
IIC: Config IIC0, speed:400000, address_width:0,master_or_slave: 1
IIC: Enable IIC0 on SCL:P12, SDA:P11
IIC: Read 1 bytes from 0x12
IIC: Read 1 bytes from 0x12, mem_addr:0x00
IIC3 read reg=00, res=e7
```
