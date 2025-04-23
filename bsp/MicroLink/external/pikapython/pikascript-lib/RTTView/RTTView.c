#include "pikaScript.h"
#include "swd_host.h"
#include "SEGGER_RTTView.h"


ATTR_RAMFUNC_WITH_ALIGNMENT(4) 
static uint8_t _acUpBuffer  [BUFFER_SIZE_UP];

ATTR_RAMFUNC_WITH_ALIGNMENT(4) 
static uint8_t _acDownBuffer[BUFFER_SIZE_DOWN];

ATTR_RAMFUNC_WITH_ALIGNMENT(4) 
static uint32_t segger_rtt_addr = 0;

#define RTT_MAX_BUFFER_SIZE 1024
uint8_t rtt_buffer[RTT_MAX_BUFFER_SIZE];

rtt_data_msg_t  tRTTMsgObj;
extern int64_t get_system_time_ms(void);

static SEGGER_RTT_CB _SEGGER_RTT;
uint32_t RTT_wAddr,RTT_wSize,RTT_wChannel;
// 读取 RTT UpBuffer 并转发
void read_rtt_and_send_usb(void)
{
    SEGGER_RTT_BUFFER_UP up_buffer;
    uint8_t buffer[16];
    static uint8_t stat = 0;
    static uint16_t read_delay_ms = 1;
    static uint8_t read_err_count = 0;
    static int64_t expected_ticks = 0;

    if (segger_rtt_addr != 0) {
        if(stat == 0) {
            if(read_err_count > 2) {
                read_err_count = 0;
                RTTView_init(RTT_wAddr, RTT_wSize);
            }
            memset(rtt_buffer, 0, sizeof(rtt_buffer));
            expected_ticks = get_system_time_ms() + read_delay_ms;
            stat = 1;
        } else if (get_system_time_ms() > expected_ticks) {
            do {
                stat = 0;

                if (swd_read_memory(segger_rtt_addr, buffer, 16)) {
                    if (memcmp(buffer, RTT_SIGNATURE, 10) != 0) {
                        read_delay_ms = 1000;
                        read_err_count++;
                        break;
                    }
                }

                uint32_t up_addr = segger_rtt_addr + offsetof(SEGGER_RTT_CB, aUp[RTT_wChannel]);

                // 读取 RTT UpBuffer 结构
                if (!swd_read_memory(up_addr, (uint8_t*)&up_buffer, sizeof(SEGGER_RTT_BUFFER_UP))){
                    read_delay_ms = 1000;
                    read_err_count++;
                    break;
                }

                uint32_t buffer_addr = (uint32_t)up_buffer.pBuffer;  // RTT 缓冲区地址
                uint32_t size = up_buffer.SizeOfBuffer;
                uint32_t wrOff = up_buffer.WrOff;
                uint32_t rdOff = up_buffer.RdOff;

                // 检查是否有新数据
                if (wrOff != rdOff) {
                    uint32_t len = (wrOff > rdOff) ? (wrOff - rdOff) : (size - rdOff);

                    if (len > RTT_MAX_BUFFER_SIZE) len = RTT_MAX_BUFFER_SIZE;

                    // **确保不会跨越缓冲区**
                    if (!swd_read_memory(buffer_addr + rdOff, rtt_buffer, len)) {
                        read_delay_ms = 1000;
                        read_err_count++;
                        break;
                    }

                    // **如果 `rdOff + len` 触及缓冲区末尾，继续读取缓冲区头部数据**
                    if (wrOff < rdOff) {
                        uint32_t second_part_len = wrOff;
                        if (second_part_len > RTT_MAX_BUFFER_SIZE - len) 
                            second_part_len = RTT_MAX_BUFFER_SIZE - len;
                        
                        if (!swd_read_memory(buffer_addr, rtt_buffer + len, second_part_len)) {
                            read_delay_ms = 1000;
                            read_err_count++;
                            break;
                        }
                        len += second_part_len;
                    }

                    emit(rtt_sig, &tRTTMsgObj,
                        args(
                            rtt_buffer,
                            len
                        ));

                    // **逐步更新 RdOff 而不是直接设为 wrOff**
                    rdOff = (rdOff + len) % size;
                    if (!swd_write_word(up_addr + offsetof(SEGGER_RTT_BUFFER_UP, RdOff), rdOff)){
                        read_delay_ms = 1000;
                        read_err_count++;
                        break;
                    }
                }
                read_delay_ms = 1;
                read_err_count = 0;
            } while(0);
        }
    }
}


uint32_t receive_usb_and_write_rtt(uint8_t inputChar)
{
    uint8_t buffer[16];
    SEGGER_RTT_BUFFER_DOWN down_buffer;

    if (segger_rtt_addr != 0) {
        do {
            uint32_t down_addr = segger_rtt_addr + offsetof(SEGGER_RTT_CB, aDown[RTT_wChannel]);

            // 读取 RTT 控制块，确保 RTT 仍然有效
            if (swd_read_memory(segger_rtt_addr, buffer, 16)) {
                if (memcmp(buffer, RTT_SIGNATURE, 10) != 0) {
                     break; 
                }
            }

            // 读取 RTT DownBuffer 结构
            if (!swd_read_memory(down_addr, (uint8_t*)&down_buffer, sizeof(SEGGER_RTT_BUFFER_DOWN))) {
                break;
            }

            uint32_t buffer_addr = (uint32_t)down_buffer.pBuffer;
            uint32_t size = down_buffer.SizeOfBuffer;
            uint32_t wrOff = down_buffer.WrOff;
            uint32_t rdOff = down_buffer.RdOff;

            // **再次读取 `RdOff` 以确认其是否已更新**
            uint32_t new_rdOff;
            if (!swd_read_word(down_addr + offsetof(SEGGER_RTT_BUFFER_DOWN, RdOff), &new_rdOff)) {
                break;
            }

            if (new_rdOff != rdOff) {
                // 说明 `RdOff` 已更新，重新检查 `wrOff` 是否仍然有效
                rdOff = new_rdOff;
            }

            // **检查缓冲区是否已满**
            uint32_t nextWrOff = (wrOff + 1) % size;
            if (nextWrOff == rdOff) {
                break; // 缓冲区满，丢弃数据
            }

            // **确保 `wrOff` 不会超出缓冲区**
            uint8_t tempBuffer[4] = { inputChar, 0, 0, 0 }; // 预留 4 字节
            if (!swd_write_memory(buffer_addr + wrOff, tempBuffer, 1)) {
                break;
            }

            // **更新 WrOff**
            wrOff = nextWrOff;
            if (!swd_write_word(down_addr + offsetof(SEGGER_RTT_BUFFER_DOWN, WrOff), wrOff)) {
                break;
            }

        } while(0);
    }
    return segger_rtt_addr;
}



void RTTView_init(uint32_t wAddr,uint32_t wSize)
{
    uint8_t buffer[16];
    segger_rtt_addr = 0;
    if (!swd_init_debug()) {
        pika_platform_printf("swd init error\r\n");
        return;
    }
    clock_cpu_delay_ms(10);
    for (uint32_t rtt_addr = wAddr; rtt_addr < wAddr + wSize; rtt_addr += 4) {
        if (swd_read_memory(rtt_addr, buffer, 16)) {
            //for(uint8_t i = 0 ;i < 16;i++){
            //    pika_platform_printf("%02x ",buffer[i]);
            //}
            //pika_platform_printf("\r\n");
            if (memcmp(buffer, RTT_SIGNATURE, 10) == 0) {
                segger_rtt_addr =  rtt_addr;// 找到了 RTT 控制块
                swd_read_memory(segger_rtt_addr, (uint8_t *)&_SEGGER_RTT, sizeof(_SEGGER_RTT));
                pika_platform_printf("Find %s addr 0x%x\r\n",_SEGGER_RTT.acID,segger_rtt_addr);
                for(uint8_t i = 0;i<_SEGGER_RTT.MaxNumUpBuffers;i++){
                    pika_platform_printf("UpBuffer Channel %d Size: %d Mode: %d\r\n",i,_SEGGER_RTT.aUp[i].SizeOfBuffer,_SEGGER_RTT.aUp[i].Flags);
                }
                for(uint8_t i = 0;i<_SEGGER_RTT.MaxNumDownBuffers;i++){
                    pika_platform_printf("DownBuffer Channel %d Size: %d Mode: %d\r\n",i,_SEGGER_RTT.aDown[i].SizeOfBuffer,_SEGGER_RTT.aDown[i].Flags);
                }

                return;
            }
            clock_cpu_delay_ms(10);
        }
    }
    pika_platform_printf("no find _SEGGER_RTT addr\r\n"); 
}

void RTTView_Uninit(void)
{
    segger_rtt_addr = 0;
}

void RTTView_start(PikaObj *self, PikaTuple* val)
{
    int val_num = pikaTuple_getSize(val);
    if(val_num == 2){
      Arg* arg = pikaTuple_getArg(val, 0);
      RTT_wAddr = (int)arg_getInt(arg);
      arg = pikaTuple_getArg(val, 1);
      RTT_wSize = (int)arg_getInt(arg);
      RTT_wChannel = 0;
    }
    if(val_num == 3){
      Arg* arg = pikaTuple_getArg(val, 0);
      RTT_wAddr = (int)arg_getInt(arg);
      arg = pikaTuple_getArg(val, 1);
      RTT_wSize = (int)arg_getInt(arg);
      arg = pikaTuple_getArg(val, 2);
      RTT_wChannel = (int)arg_getInt(arg);
    }
    pika_platform_printf("Addr = 0x%x,wSize = %d,Channel = %d\n",RTT_wAddr,RTT_wSize,RTT_wChannel);
    RTTView_init(RTT_wAddr,RTT_wSize);
}


void RTTView_stop(PikaObj *self, PikaTuple* val)
{
    segger_rtt_addr = 0;
}
