#include <stdio.h>
#include <hpm_gpiom_soc_drv.h>
#include <hpm_gpio_drv.h>
#include <hpm_gpiom_drv.h>
#include "board.h"
#include "hpm_uart_drv.h"
#include "hpm_dma_mgr.h"
#include "usb2uart.h"
#include "chry_ringbuffer.h"

#define UART_BASE                  HPM_UART2
#define UART_IRQ                   IRQn_UART2
#define UART_CLK_NAME              clock_uart2
#define UART_RX_DMA                HPM_DMA_SRC_UART2_RX
#define UART_RX_DMA_RESOURCE_INDEX (0U)
#define UART_RX_DMA_BUFFER_SIZE    (4096U)
#define UART_RX_DMA_BUFFER_COUNT   (2)

#define UART_TX_DMA                HPM_DMA_SRC_UART2_TX
#define UART_TX_DMA_RESOURCE_INDEX (1U)

static uint32_t rb_write_pos = 0;
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(4)
static uint8_t uart_rx_buf[UART_RX_DMA_BUFFER_COUNT][UART_RX_DMA_BUFFER_SIZE];
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(8)
static dma_linked_descriptor_t rx_descriptors[UART_RX_DMA_BUFFER_COUNT];

static dma_resource_t dma_resource_pools[2];
static volatile uint32_t g_uart_tx_transfer_length = 0;
static hpm_stat_t board_uart_dma_config(void);
static void chry_dap_usb2uart_uart_send_complete(uint32_t size);

USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_uartrx;
USB_NOCACHE_RAM_SECTION chry_ringbuffer_t g_usbrx;

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t uartrx_ringbuffer[CONFIG_UARTRX_RINGBUF_SIZE];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usbrx_ringbuffer[CONFIG_USBRX_RINGBUF_SIZE];

uart_data_msg_t  tUartMsgObj;
volatile uint8_t config_uart = 0;
volatile struct cdc_line_coding g_cdc_lincoding;
volatile uint8_t config_uart_transfer = 0;
volatile uint8_t usbrx_idle_flag = 0;
volatile uint8_t usbtx_idle_flag = 0;
volatile uint8_t uarttx_idle_flag = 0;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_tmpbuffer[DAP_PACKET_SIZE];
extern void led_usb_in_activity(void);
extern void led_usb_out_activity(void);
static void uart_dma_channel_tc_callback(DMA_Type *ptr, uint32_t channel, void *user_data)
{
    (void)ptr;
    (void)channel;
    (void)user_data;
    uint32_t i;
    dma_resource_t *rx_resource = &dma_resource_pools[UART_RX_DMA_RESOURCE_INDEX];
    dma_resource_t *tx_resource = &dma_resource_pools[UART_TX_DMA_RESOURCE_INDEX];
    uint32_t link_addr = ptr->CHCTRL[channel].LLPOINTER;
    uint32_t rx_desc_size = (sizeof(rx_descriptors) / sizeof(dma_linked_descriptor_t));
    if (rx_resource->channel == channel) {
        for (i = 0; i < rx_desc_size; i++) {
            if (link_addr == core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&rx_descriptors[i])) {
                emit(uart_sig, &tUartMsgObj,
                     args(
                          &uart_rx_buf[i][rb_write_pos],
                          (UART_RX_DMA_BUFFER_SIZE - rb_write_pos)
                     ));
                rb_write_pos = 0;
            }
        }
    }
    if (tx_resource->channel == channel) {
        chry_dap_usb2uart_uart_send_complete(g_uart_tx_transfer_length);
    }
}

SDK_DECLARE_EXT_ISR_M(UART_IRQ, uart_isr)
void uart_isr(void)
{
    uint32_t uart_received_data_count;
    uint32_t i;
    dma_resource_t *rx_resource = &dma_resource_pools[UART_RX_DMA_RESOURCE_INDEX];
    uint32_t link_addr = rx_resource->base->CHCTRL[rx_resource->channel].LLPOINTER;
    uint32_t rx_desc_size = (sizeof(rx_descriptors) / sizeof(dma_linked_descriptor_t));
    if (uart_is_rxline_idle(UART_BASE)) {
        uart_clear_rxline_idle_flag(UART_BASE);
        uart_received_data_count = UART_RX_DMA_BUFFER_SIZE - dma_get_remaining_transfer_size(rx_resource->base, rx_resource->channel);
        if (uart_received_data_count > 0) {
            for (i = 0; i < rx_desc_size; i++) {
                if (link_addr == core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&rx_descriptors[i])) {
                    emit(uart_sig, &tUartMsgObj,
                         args(
                              &uart_rx_buf[i][rb_write_pos],
                              (uart_received_data_count - rb_write_pos)
                         ));
                    rb_write_pos += uart_received_data_count - rb_write_pos;
                    break;
                }
            }
        }
    }
    if (uart_is_txline_idle(UART_BASE)) {
        uart_clear_txline_idle_flag(UART_BASE);
        gpio_write_pin(BOARD_EN_GPIO_CTRL, BOARD_EN_GPIO_INDEX, BOARD_EN_GPIO_PIN, 0);
    }
}


void usb2uart_handler (void)
{
    dma_resource_t *rx_resource = &dma_resource_pools[UART_RX_DMA_RESOURCE_INDEX];
    const uint32_t rx_desc_size = (sizeof(rx_descriptors) / sizeof(dma_linked_descriptor_t));
    uint32_t uart_received_data_count = UART_RX_DMA_BUFFER_SIZE - dma_get_remaining_transfer_size(rx_resource->base, rx_resource->channel);

    if ((uart_received_data_count - rb_write_pos) > 0)
    {
        uint32_t level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
        uint32_t link_addr = rx_resource->base->CHCTRL[rx_resource->channel].LLPOINTER;

        /* data may be preempted by interrupts, need to reread the count */
        uart_received_data_count = UART_RX_DMA_BUFFER_SIZE - dma_get_remaining_transfer_size(rx_resource->base, rx_resource->channel);

        if ((uart_received_data_count - rb_write_pos) > 0)
        {
            for (uint32_t i = 0; i < rx_desc_size; i++)
            {
                if (link_addr == core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&rx_descriptors[i]))
                {
                    emit(uart_sig, &tUartMsgObj,
                         args(
                              &uart_rx_buf[i][rb_write_pos],
                              (uart_received_data_count - rb_write_pos)
                         ));
                    rb_write_pos += uart_received_data_count - rb_write_pos;
                    break;
                }
            }
        }

        restore_global_irq(level);
    }
}

void uartx_preinit(void)
{
    chry_ringbuffer_init(&g_uartrx, uartrx_ringbuffer, CONFIG_UARTRX_RINGBUF_SIZE);
    chry_ringbuffer_init(&g_usbrx, usbrx_ringbuffer, CONFIG_USBRX_RINGBUF_SIZE);

    board_init_uart(UART_BASE);
    clock_set_source_divider(UART_CLK_NAME, clk_src_pll1_clk0, 8);
    clock_add_to_group(UART_CLK_NAME, 0);
    intc_m_enable_irq_with_priority(UART_IRQ, 3);
    uart_clear_rxline_idle_flag(UART_BASE);
    if (board_uart_dma_config() != status_success) {
        return;
    }
}

static void chry_dap_usb2uart_uart_config_callback(struct cdc_line_coding *line_coding)
{
    uart_config_t config = { 0 };
    uart_default_config(UART_BASE, &config);
    config.baudrate = line_coding->dwDTERate;
    config.parity = line_coding->bParityType;
    config.word_length = line_coding->bDataBits - 5;
    config.num_of_stop_bits = line_coding->bCharFormat;
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(UART_CLK_NAME);
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty; /* this config should not change */
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rxidle_config.detect_enable = true;
    config.rxidle_config.detect_irq_enable = true;
    config.rxidle_config.idle_cond = uart_rxline_idle_cond_state_machine_idle;
    config.rxidle_config.threshold = 30U; /* 20bit */

    config.txidle_config.detect_enable = true;
    config.txidle_config.detect_irq_enable  = true;
    config.txidle_config.idle_cond = uart_rxline_idle_cond_state_machine_idle;
    config.txidle_config.threshold = 3U;

    uart_init(UART_BASE, &config);
    uart_init_txline_idle_detection(UART_BASE, config.txidle_config);
    uart_clear_rxline_idle_flag(UART_BASE);
    uart_clear_txline_idle_flag(UART_BASE);
    uart_reset_rx_fifo(UART_BASE);
    uart_reset_tx_fifo(UART_BASE);

}

static void chry_dap_usb2uart_uart_send_bydma(uint8_t *data, uint16_t len)
{
    uint32_t buf_addr;
    dma_resource_t *tx_resource = &dma_resource_pools[UART_TX_DMA_RESOURCE_INDEX];
    if (len <= 0) {
        return;
    }
    gpio_write_pin(BOARD_EN_GPIO_CTRL, BOARD_EN_GPIO_INDEX, BOARD_EN_GPIO_PIN, 1);
    g_uart_tx_transfer_length = len;
    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)data);
    dma_mgr_set_chn_src_addr(tx_resource, buf_addr);
    dma_mgr_set_chn_transize(tx_resource, len);
    dma_mgr_enable_channel(tx_resource);
}

static hpm_stat_t board_uart_dma_config(void)
{
    dma_mgr_chn_conf_t chg_config;
    dma_resource_t *resource = NULL;
    uint32_t i = 0;
    uint32_t rx_desc_size = (sizeof(rx_descriptors) / sizeof(dma_linked_descriptor_t));
    dma_mgr_get_default_chn_config(&chg_config);
    chg_config.src_width = DMA_MGR_TRANSFER_WIDTH_BYTE;
    chg_config.dst_width = DMA_MGR_TRANSFER_WIDTH_BYTE;
    /* uart rx dma config */
    resource = &dma_resource_pools[UART_RX_DMA_RESOURCE_INDEX];
    if (dma_mgr_request_resource(resource) == status_success) {
        chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
        chg_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
        chg_config.src_addr = (uint32_t)&UART_BASE->RBR;
        chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
        chg_config.dst_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
        chg_config.size_in_byte = UART_RX_DMA_BUFFER_SIZE;
        chg_config.en_dmamux = true;
        chg_config.dmamux_src = UART_RX_DMA;
        for (i = 0; i < rx_desc_size; i++) {
            if (i < (rx_desc_size - 1)) {
                chg_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf[i]);
                chg_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&rx_descriptors[i]);
            } else {
                chg_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf[0]);
                chg_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&rx_descriptors[0]);
            }
            if (dma_mgr_config_linked_descriptor(resource, &chg_config, (dma_mgr_linked_descriptor_t *)&rx_descriptors[i]) != status_success) {
                printf("generate dma desc fail\n");
                return status_fail;
            }
            rx_descriptors[i].ctrl &= ~DMA_MGR_INTERRUPT_MASK_TC;
            printf("linker_addr:0x%08x  %d\n", (uint32_t)&rx_descriptors[i], i);
        }
        chg_config.dst_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)uart_rx_buf[0]);
        chg_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&rx_descriptors[0]);
        dma_mgr_setup_channel(resource, &chg_config);
        dma_mgr_enable_channel(resource);
        dma_mgr_install_chn_tc_callback(resource, uart_dma_channel_tc_callback, NULL);
        dma_mgr_enable_chn_irq(resource, DMA_MGR_INTERRUPT_MASK_TC);
        dma_mgr_enable_dma_irq_with_priority(resource, 1);
    }
    /* uart tx dma config */
    resource = &dma_resource_pools[UART_TX_DMA_RESOURCE_INDEX];
    if (dma_mgr_request_resource(resource) == status_success) {
        chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
        chg_config.src_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
        chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
        chg_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
        chg_config.dst_addr = (uint32_t)&UART_BASE->THR;
        chg_config.en_dmamux = true;
        chg_config.dmamux_src = UART_TX_DMA;
        chg_config.linked_ptr = (uint32_t)NULL;
        dma_mgr_setup_channel(resource, &chg_config);
        dma_mgr_install_chn_tc_callback(resource, uart_dma_channel_tc_callback, NULL);
        dma_mgr_enable_chn_irq(resource, DMA_MGR_INTERRUPT_MASK_TC);
        dma_mgr_enable_dma_irq_with_priority(resource, 1);
    }
    return status_success;
}

static void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void) busid;
    (void) ep;
    led_usb_out_activity();
    chry_ringbuffer_write(&g_usbrx, cdc_tmpbuffer, nbytes);
    if (chry_ringbuffer_get_free(&g_usbrx) >= DAP_PACKET_SIZE) {
        usbd_ep_start_read(0, ep, cdc_tmpbuffer, DAP_PACKET_SIZE);
    } else {
        usbrx_idle_flag = 1;
    }
}

static void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void) busid;
    (void) ep;
    uint32_t size;
    uint8_t *buffer;
    led_usb_in_activity();
    chry_ringbuffer_linear_read_done(&g_uartrx, nbytes);
    if ((nbytes % DAP_PACKET_SIZE) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(0, ep, NULL, 0);
    } else {
        if (chry_ringbuffer_get_used(&g_uartrx)) {
            buffer = chry_ringbuffer_linear_read_setup(&g_uartrx, &size);
            usbd_ep_start_write(0, ep, buffer, size);
        } else {
            usbtx_idle_flag = 1;
        }
    }
}

struct usbd_endpoint cdc_out_ep = {
        .ep_addr = CDC_OUT_EP,
        .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep = {
        .ep_addr = CDC_IN_EP,
        .ep_cb = usbd_cdc_acm_bulk_in
};


/* called by user */
static void chry_dap_usb2uart_uart_send_complete(uint32_t size)
{
    uint8_t *buffer;

    chry_ringbuffer_linear_read_done(&g_usbrx, size);

    if (chry_ringbuffer_get_used(&g_usbrx)) {
        buffer = chry_ringbuffer_linear_read_setup(&g_usbrx, &size);
        chry_dap_usb2uart_uart_send_bydma(buffer, size);
    } else {
        uarttx_idle_flag = 1;
    }
}

void chry_dap_usb2uart_handle(void)
{
    uint32_t size;
    uint8_t *buffer;

    if (config_uart) {
        /* disable irq here */
        config_uart = 0;
        /* config uart here */
        chry_dap_usb2uart_uart_config_callback((struct cdc_line_coding *) &g_cdc_lincoding);
        usbtx_idle_flag = 1;
        uarttx_idle_flag = 1;
        config_uart_transfer = 1;
        //chry_ringbuffer_reset_read(&g_uartrx);
        /* enable irq here */
    }

    if (config_uart_transfer == 0) {
        return;
    }

    /* why we use chry_ringbuffer_linear_read_setup?
     * becase we use dma and we do not want to use temp buffer to memcpy from ringbuffer
     * 
    */

    /* uartrx to usb tx */
    if (usbtx_idle_flag) {
        if (chry_ringbuffer_get_used(&g_uartrx)) {
            usbtx_idle_flag = 0;
            /* start first transfer */
            buffer = chry_ringbuffer_linear_read_setup(&g_uartrx, &size);
            usbd_ep_start_write(0, CDC_IN_EP, buffer, size);
        }
    }

    /* usbrx to uart tx */
    if (uarttx_idle_flag) {
        if (chry_ringbuffer_get_used(&g_usbrx)) {
            uarttx_idle_flag = 0;
            /* start first transfer */
            buffer = chry_ringbuffer_linear_read_setup(&g_usbrx, &size);
            chry_dap_usb2uart_uart_send_bydma(buffer, size);
        }
    }

    /* check whether usb rx ringbuffer have space to store */
    if (usbrx_idle_flag) {
        if (chry_ringbuffer_get_free(&g_usbrx) >= DAP_PACKET_SIZE) {
            usbrx_idle_flag = 0;
            usbd_ep_start_read(0, CDC_OUT_EP, cdc_tmpbuffer, DAP_PACKET_SIZE);
        }
    }
}


