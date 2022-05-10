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

#include "pico_uac2_program.h"
#include "bsp/board.h"
#include <cstdio>
#include "pico_i2s.h"
#include "si5351.h"
#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
#include "cs4398.h"
#endif

#ifdef DAC_DacPlusPro
#include "DacPlusPro.h"
#endif

volatile uint8_t spk_buf_cnt = 0;
volatile bool uac_runnnig = false;

volatile uint32_t spk_recieved_buf_size[spk_buf_size];

volatile int feedback = 0x300000;

volatile int bit_uac2 = 0;
volatile int freq_uac2 = 0;

void pico_uac2_init()
{
#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
    change_volume_cs4398(100);
#endif

#ifdef DAC_DacPlusPro
    change_volume_DacPlusPro(100);
#endif
}

void stop_i2s()
{
    if (uac_runnnig)
    {
        deinit_i2s();
        spk_buf_cnt = 0;
        int_count_i2s = 0;
        i2s_buff_count = 0;
        uac_runnnig = false;
    }

    bit_uac2 = 0;
    freq_uac2 = 0;
}

void bit_sample_rate_changed(int bit, int sample_rate)
{
    bit_uac2 = bit;
    freq_uac2 = sample_rate;
    if (uac_runnnig)
    {
        if (i2s_buff)
        {
            deinit_i2s();
        }
    }
    printf("\nsample rate = %d\n\n", sample_rate);
    if (sample_rate % 48000 == 0)
    {
        feedback = 0x300000;
    }
    else
    {
        feedback = 0x2c2000;
    }
}

volatile int count_tmp = 0;

void got_audio_buffer(int bytes_received)
{
    if (!i2s_buff)
    {
        i2s_buff_size = 1024 * 6;
        i2s_buff_num = i2s_buf_size_uac2;
        printf("bit = %d\n", bit_uac2);

        if (init_i2s(bit_uac2) != 0)
        {
            bit_uac2 = -1;
            freq_uac2 = -1;
        }
    }

    if (bit_uac2 == 24)
    {
        uint8_t *spk_buf_8 = (uint8_t *)&spk_buf[spk_buf_cnt][0];
        uint8_t *buff_8 = (uint8_t *)&i2s_buff[(i2s_buff_count + 0) % i2s_buf_size_uac2][0];
        volatile int count = 0;

        while ((count < bytes_received) && (count_tmp < i2s_buff_size * 2))
        {
            buff_8[count_tmp + 0] = spk_buf_8[count + 2];
            buff_8[count_tmp + 1] = spk_buf_8[count + 1];
            buff_8[count_tmp + 2] = spk_buf_8[count + 0];
            count += 3;
            count_tmp += 3;
        }

        if (count_tmp >= i2s_buff_size * 2)
        {
            i2s_buff_count++;
            count_tmp = 0;
            buff_8 = (uint8_t *)&i2s_buff[(i2s_buff_count + 0) % i2s_buf_size_uac2][0];
            if (count >= bytes_received)
            {
            }
            else
            {
                while ((count < bytes_received) && (count_tmp < i2s_buff_size * 2))
                {
                    buff_8[count_tmp + 0] = spk_buf_8[count + 2];
                    buff_8[count_tmp + 1] = spk_buf_8[count + 1];
                    buff_8[count_tmp + 2] = spk_buf_8[count + 0];
                    count += 3;
                    count_tmp += 3;
                }
            }
        }
    }
    else
    {
        int16_t *spk_buf_16 = (int16_t *)&spk_buf[spk_buf_cnt][0];
        volatile int count = 0;
        while ((count < bytes_received / 2) && (count_tmp < i2s_buff_size))
        {
            i2s_buff[(i2s_buff_count + 0) % i2s_buf_size_uac2][count_tmp++] = spk_buf_16[count++];
        }

        if (count_tmp >= i2s_buff_size)
        {
            i2s_buff_count++;
            count_tmp = 0;
            if (count >= bytes_received / 2)
            {
            }
            else
            {
                while ((count < bytes_received / 2) && (count_tmp < i2s_buff_size))
                {
                    i2s_buff[(i2s_buff_count + 0) % i2s_buf_size_uac2][count_tmp++] = spk_buf_16[count++];
                }
            }
        }
    }

    spk_buf_cnt++;
    spk_buf_cnt %= spk_buf_size;
}

void audio_task_pico(int resolution, int sample_rate)
{
    static bool feedback_on = false;
    static int time_millis = 0;
    static int time_millis_start = 0;
    static int delta_pre = 0;

    if (!uac_runnnig)
    {
        if (i2s_buff_count > i2s_buf_size_uac2 / 2)
        {
#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
            cs4398_set_FM(sample_rate);
#endif

#ifdef DAC_DacPlusPro
            DacPlusPro_change_bit_freq(resolution, sample_rate);
#endif

#ifdef DAC_DacPlusPro

#else
            si5351_set_clock(resolution, sample_rate);
#endif

            i2s_start();
            uac_runnnig = true;
            time_millis = board_millis();
            time_millis_start = board_millis();
        }
    }

    if (uac_runnnig)
    {
        if ((board_millis() - time_millis) > 1000)
        {
            // printf("uac = %ld,i2s = %ld, %d\n", i2s_buff_count, int_count_i2s, (i2s_buff_count - int_count_i2s));
            printf("%d\n", (i2s_buff_count - int_count_i2s));
            time_millis = board_millis();
            int delta = ((i2s_buff_count - int_count_i2s) % i2s_buf_size_uac2) - i2s_buf_size_uac2 / 2;
            while (delta < (-i2s_buf_size_uac2))
            {
                delta += i2s_buf_size_uac2;
            }

            if (delta > 1 || delta < -1)
            {
                feedback_on = true;
                if (delta_pre != delta)
                {
                    uint32_t feedback_data = feedback - delta * feedback_coefficient;

                    if (sample_rate % 48000 == 0)
                    {
                        feedback_data *= (sample_rate / 48000);
                    }
                    else
                    {
                        feedback_data *= (sample_rate / 44100);
                    }

                    tud_audio_fb_set(feedback_data);
                    // printf("feedback = %x\n", feedback_data);
                }
            }
            else
            {
                if (feedback_on)
                {
                    uint32_t feedback_data = feedback;

                    if (sample_rate % 48000 == 0)
                    {
                        feedback_data *= (sample_rate / 48000);
                    }
                    else
                    {
                        feedback_data *= (sample_rate / 44100);
                    }

                    tud_audio_fb_set(feedback_data);
                    // printf("feedback = %x\n", feedback_data);

                    feedback_on = false;
                }
            }
            delta_pre = delta;
        }
    }
}