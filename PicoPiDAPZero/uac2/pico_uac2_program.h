/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 nzc21489
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef PICO_UAC2_PROGRAM_H
#define PICO_UAC2_PROGRAM_H

#include "pico/stdlib.h"

#include "uac2_config.h"

#include "tusb.h"
#include "usb_descriptors.h"

extern int32_t spk_buf[spk_buf_size][CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 4];
extern volatile uint8_t spk_buf_cnt;

extern volatile int int_count_uac2;
extern volatile bool uac_runnnig;

extern volatile uint32_t spk_recieved_buf_size[spk_buf_size];

extern volatile int feedback;

extern volatile int bit_uac2;
extern volatile int freq_uac2;

extern volatile bool usb_dac_mode;

void __isr __time_critical_func(dma_handler_i2s0)();
void __isr __time_critical_func(dma_handler_i2s1)();
void pico_uac2_init();
void stop_i2s();
void bit_sample_rate_changed(int bit, int sample_rate);
void got_audio_buffer(int bytes_received);
void audio_task_pico(int resolution, int sample_rate);

#endif // PICO_UAC2_PROGRAM_H