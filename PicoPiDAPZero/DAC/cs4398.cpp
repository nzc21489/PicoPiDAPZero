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
#include "pico_i2c.h"

cs4398::cs4398()
{
    
}

void cs4398::set_dac_address(uint8_t address)
{
    dac_address = address;
}

void cs4398::set_i2c_port(i2c_inst_t i2c_port)
{
    dac_i2c_port = i2c_port;
}

void cs4398::setup()
{
    int timeout = 1000;
    int count = 0;
    int bw;

    do
    {
        bw = write_i2c(dac_i2c_port, dac_address, 8, register_value[8]);
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
        reg_datas[i] = register_value[i + 1];
    }
    do
    {
        bw = write_i2c_multi(dac_i2c_port, dac_address, reg_datas, 9);
        count++;
        if (count > timeout)
        {
            break;
        }
    } while (bw != 9);

    // clear PDN bit
    register_value[8] &= 0b01111111;
    write_i2c(dac_i2c_port, dac_address, 8, register_value[8]);
}

void cs4398::mute()
{
    register_value[4] |= 0b00010000;

    int bw = write_i2c(dac_i2c_port, dac_address, 4, register_value[4]);

    // clear PDN bit
    register_value[8] &= 0b01111111;

    write_i2c(dac_i2c_port, dac_address, 8, register_value[8]);
}

void cs4398::unmute()
{
    register_value[4] &= 0b11101111;
    int bw = write_i2c(dac_i2c_port, dac_address, 4, register_value[4]);
}

void cs4398::set_FM(int sampling_rate)
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

    register_value[2] &= 0b11111100;
    register_value[2] |= audio_sample;

    int bw = write_i2c(dac_i2c_port, dac_address, 2, register_value[2]);
}

void cs4398::set_bit_freq(uint8_t bit, uint32_t freq)
{
    set_FM(freq);
}

bool cs4398::set_volume(uint8_t vol)
{
    uint8_t vol_value_send = (vol_max - vol_min) * vol / 100;
    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    write_i2c(dac_i2c_port, dac_address, 5, vol_value_send);
    write_i2c(dac_i2c_port, dac_address, 6, vol_value_send);
    return true;
}

uint8_t cs4398::get_digital_filter_num()
{
    return digital_filter_num;
}

string cs4398::get_digital_filter_strs(uint8_t filter_num)
{
    return digital_filter_strs[filter_num];
}

void cs4398::set_digital_filter(int filter_num)
{
    if (filter_num < digital_filter_num)
    {
        register_value[7] &= 0b11111011;
        register_value[7] |= (digital_filter_nums[filter_num] << 2);
        write_i2c(dac_i2c_port, dac_address, 7, register_value[7]);
    }
}

string cs4398::get_digital_filter_text_name()
{
    return digital_filter_text;
}

dac_type cs4398::get_dac()
{
    return dac_cs4398;
}