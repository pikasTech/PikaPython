/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_gptmr_drv.h"

void gptmr_channel_get_default_config(GPTMR_Type *ptr, gptmr_channel_config_t *config)
{
    (void) ptr;
    config->mode = gptmr_work_mode_no_capture;
    config->dma_request_event = gptmr_dma_request_disabled;
    config->synci_edge = gptmr_synci_edge_none;
    for (uint8_t i = 0; i < GPTMR_CH_CMP_COUNT; i++) {
        config->cmp[i] = 0xFFFFFFFEUL;
    }
    config->reload = 0xFFFFFFFEUL;
    config->cmp_initial_polarity_high = true;
    config->enable_cmp_output = true;
    config->enable_sync_follow_previous_channel = false;
    config->enable_software_sync = false;
    config->debug_mode = true;

#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE  == 1)
    config->counter_mode = gptmr_counter_mode_internal;
#endif

#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE  == 1)
    config->enable_opmode = false;
#endif

#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
    config->enable_monitor = false;
    gptmr_channel_get_default_monitor_config(ptr, &config->monitor_config);
#endif
}

hpm_stat_t gptmr_channel_config(GPTMR_Type *ptr,
                         uint8_t ch_index,
                         gptmr_channel_config_t *config,
                         bool enable)
{
    uint32_t v = 0;
    uint32_t tmp_value;

    if (config->enable_sync_follow_previous_channel && !ch_index) {
        return status_invalid_argument;
    }

    if (config->dma_request_event != gptmr_dma_request_disabled) {
        v |= GPTMR_CHANNEL_CR_DMAEN_MASK
            | GPTMR_CHANNEL_CR_DMASEL_SET(config->dma_request_event);
    }
    v |= GPTMR_CHANNEL_CR_CAPMODE_SET(config->mode)
        | GPTMR_CHANNEL_CR_DBGPAUSE_SET(config->debug_mode)
        | GPTMR_CHANNEL_CR_SWSYNCIEN_SET(config->enable_software_sync)
        | GPTMR_CHANNEL_CR_CMPINIT_SET(config->cmp_initial_polarity_high)
        | GPTMR_CHANNEL_CR_SYNCFLW_SET(config->enable_sync_follow_previous_channel)
        | GPTMR_CHANNEL_CR_CMPEN_SET(config->enable_cmp_output)
        | GPTMR_CHANNEL_CR_CEN_SET(enable)
        | config->synci_edge;
#if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE  == 1)
    v |= GPTMR_CHANNEL_CR_CNT_MODE_SET(config->counter_mode);
#endif
#if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE  == 1)
    v |= GPTMR_CHANNEL_CR_OPMODE_SET(config->enable_opmode);
#endif
    for (uint8_t i = GPTMR_CH_CMP_COUNT; i > 0; i--) {
        tmp_value = config->cmp[i - 1];
        if ((tmp_value > 0)  && (tmp_value != 0xFFFFFFFFu)) {
            tmp_value--;
        }
        ptr->CHANNEL[ch_index].CMP[i - 1] = GPTMR_CHANNEL_CMP_CMP_SET(tmp_value);
    }
    tmp_value = config->reload;
    if ((tmp_value > 0) && (tmp_value != 0xFFFFFFFFu)) {
        tmp_value--;
    }
    ptr->CHANNEL[ch_index].RLD = GPTMR_CHANNEL_RLD_RLD_SET(tmp_value);
    ptr->CHANNEL[ch_index].CR = v;
#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
    gptmr_channel_monitor_config(ptr, ch_index, &config->monitor_config, config->enable_monitor);
#endif

    return status_success;
}

#if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR  == 1)
void gptmr_channel_get_default_monitor_config(GPTMR_Type *ptr, gptmr_channel_monitor_config_t *config)
{
    (void) ptr;
    config->max_value = 0;
    config->min_value = 0;
    config->monitor_type = monitor_signal_high_level_time;
}

hpm_stat_t gptmr_channel_monitor_config(GPTMR_Type *ptr, uint8_t ch_index, gptmr_channel_monitor_config_t *config, bool enable)
{
    if ((ptr == NULL) || (config->max_value < config->min_value)) {
        return status_invalid_argument;
    }
    if (enable == true) {
        gptmr_channel_set_monitor_type(ptr, ch_index, config->monitor_type);
        gptmr_update_cmp(ptr, ch_index, 0, config->min_value);
        gptmr_update_cmp(ptr, ch_index, 1, config->max_value);
        gptmr_channel_config_update_reload(ptr, ch_index, 0xFFFFFFFF);
        gptmr_channel_set_capmode(ptr, ch_index, gptmr_work_mode_measure_width);
        gptmr_channel_reset_count(ptr, ch_index);
        gptmr_channel_enable_monitor(ptr, ch_index);
    } else {
        gptmr_channel_disable_monitor(ptr, ch_index);
    }
    return status_success;
}

#endif
