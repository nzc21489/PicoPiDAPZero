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

#include "cs4398.h"

#include "hardware/i2c.h"

#define i2c_port_cs4398 i2c1

uint8_t cs4398_register_value[10] = {
    0b00000000, // dummy
    0b01110000, // 1
    0b00010000, // 2
    0b00001001, // 3
    0b00000000, // 4
    0b00000000, // 5
    0b00000000, // 6
    0b10110000, // 7
    0b11000000, // 8
    0b00001000, // 9
};

int send_i2c(uint8_t reg, uint8_t value)
{
    uint8_t reg_data[2];
    reg_data[0] = reg;
    reg_data[1] = value;
    int bw = i2c_write_blocking(i2c_port_cs4398, cs4398address, &reg_data[0], 2, false);
    return bw;
}

int send_i2c_multi(uint8_t *value, int num)
{
    int bw = i2c_write_blocking(i2c_port_cs4398, cs4398address, value, num, false);
    return bw;
}

void cs4398_setup()
{
    int timeout = 1000;
    int count = 0;
    int bw;

    do
    {
        bw = send_i2c(8, cs4398_register_value[8]);
        count++;
        if (count > timeout)
        {
            break;
        }
    } while (bw != 2);

    count = 0;

    uint8_t reg_datas[9];
    reg_datas[0] = 2 | 0b10000000;
    for (int i = 1; i < 9; i++)
    {
        reg_datas[i] = cs4398_register_value[i + 1];
    }
    do
    {
        bw = send_i2c_multi(reg_datas, 9);
        count++;
        if (count > timeout)
        {
            break;
        }
    } while (bw != 9);

    // clear PDN bit
    cs4398_register_value[8] &= 0b01111111;
    send_i2c(8, cs4398_register_value[8]);
}

void cs4398_mute()
{
    cs4398_register_value[4] |= 0b00010000;

    int bw = send_i2c(4, cs4398_register_value[4]);

    // clear PDN bit
    cs4398_register_value[8] &= 0b01111111;

    send_i2c(8, cs4398_register_value[8]);
}

void cs4398_unmute()
{
    cs4398_register_value[4] &= 0b11101111;
    int bw = send_i2c(4, cs4398_register_value[4]);
}

void cs4398_set_FM(int sampling_rate)
{
    uint8_t audio_sample = 0;

    if (sampling_rate % 48000 == 0)
    {
        switch (sampling_rate / 48000)
        {
        case 1:
            audio_sample = 0;
            break;

        case 2:
            audio_sample = 1;
            break;

        case 4:
            audio_sample = 2;
            break;

        case 8:
            audio_sample = 2;
            break;

        default:
            break;
        }
    }
    else
    {
        if (sampling_rate % 44100 == 0)
        {
            switch (sampling_rate / 44100)
            {
            case 1:
                audio_sample = 0;
                break;

            case 2:
                audio_sample = 1;
                break;

            case 4:
                audio_sample = 2;
                break;

            case 8:
                audio_sample = 2;
                break;

            default:
                break;
            }
        }
    }

    cs4398_register_value[2] &= 0b11111100;
    cs4398_register_value[2] |= audio_sample;

    int bw = send_i2c(2, cs4398_register_value[2]);
}

void change_volume_cs4398(uint8_t vol)
{
    uint8_t vol_value_send = (vol_max - vol_min) * vol / 100;
    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    send_i2c(5, vol_value_send);
    send_i2c(6, vol_value_send);
}
