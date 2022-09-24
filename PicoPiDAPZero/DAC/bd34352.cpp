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

#include "bd34352.h"

#include "pico_i2c.h"

bd34352::bd34352()
{
    
}

void bd34352::set_dac_address(uint8_t address)
{
    dac_address = address;
}

void bd34352::set_i2c_port(i2c_inst_t i2c_port)
{
    dac_i2c_port = i2c_port;
}

void bd34352::setup()
{
    // clock1
    write_i2c(dac_i2c_port, dac_address, 0x4, 0b0);

    // clock2
    write_i2c(dac_i2c_port, dac_address, 0x6, 0b0);

    // audio i/f 1
    write_i2c(dac_i2c_port, dac_address, 0x10, 0b1011);

    // audio i/f 2
    write_i2c(dac_i2c_port, dac_address, 0x12, 0b0);

    // audio i/f 3
    write_i2c(dac_i2c_port, dac_address, 0x13, 0b0);

    // audio output polality
    write_i2c(dac_i2c_port, dac_address, 0x14, 0b0);

    // dsd filter
    write_i2c(dac_i2c_port, dac_address, 0x16, 0b110);

    // audio input polarity
    write_i2c(dac_i2c_port, dac_address, 0x17, 0b0);

    // volume transision time
    write_i2c(dac_i2c_port, dac_address, 0x20, 0b1111000);

    // volume 1
    write_i2c(dac_i2c_port, dac_address, 0x21, 0b11111111); // mute

    // volume 2
    write_i2c(dac_i2c_port, dac_address, 0x22, 0b11111111); // mute

    // mute transition time
    write_i2c(dac_i2c_port, dac_address, 0x29, 0b1000);

    // fir filter 1
    write_i2c(dac_i2c_port, dac_address, 0x30, 0b0001);

    // fir filter 2
    write_i2c(dac_i2c_port, dac_address, 0x31, 0b0);

    // de-emphasis 1
    write_i2c(dac_i2c_port, dac_address, 0x33, 0b0);

    // de-emphasis 2
    write_i2c(dac_i2c_port, dac_address, 0x34, 0b0);

    // delta sigma
    write_i2c(dac_i2c_port, dac_address, 0x40, 0b10001);

    // 41h
    write_i2c(dac_i2c_port, dac_address, 0x41, 0b0);

    // 42h
    write_i2c(dac_i2c_port, dac_address, 0x42, 0b10110);

    // 43h
    write_i2c(dac_i2c_port, dac_address, 0x43, 0b10);

    // 48h
    write_i2c(dac_i2c_port, dac_address, 0x48, 0b0);

    // setting 5
    write_i2c(dac_i2c_port, dac_address, 0x60, 0b10110);

    // setting 6
    write_i2c(dac_i2c_port, dac_address, 0x61, 0b10110);

    // software reset off
    write_i2c(dac_i2c_port, dac_address, 0x0, 0b1);

    // digital power on
    write_i2c(dac_i2c_port, dac_address, 0x2, 0b1);

    // pop noise prevention
    write_i2c(dac_i2c_port, dac_address, 0xd0, 0b1101010);

    // pop noise prevention
    write_i2c(dac_i2c_port, dac_address, 0xd3, 0b10000);

    // pop noise prevention
    write_i2c(dac_i2c_port, dac_address, 0xd3, 0b0);

    // pop noise prevention
    write_i2c(dac_i2c_port, dac_address, 0xd0, 0b0);

    // analog power on
    write_i2c(dac_i2c_port, dac_address, 0x3, 0b1);

    // ram clear on
    write_i2c(dac_i2c_port, dac_address, 0x2f, 0b10000000);

    // ram clear off
    write_i2c(dac_i2c_port, dac_address, 0x2f, 0b0);

    // mute off
    write_i2c(dac_i2c_port, dac_address, 0x2a, 0b11);
}

void bd34352::mute()
{
    write_i2c(dac_i2c_port, dac_address, 0x2a, 0b0);
}

void bd34352::unmute()
{
    write_i2c(dac_i2c_port, dac_address, 0x2a, 0b11);
}

void bd34352::set_bit_freq(uint8_t bit, uint32_t freq)
{
    sampling_rate = freq;

    // mute
    write_i2c(dac_i2c_port, dac_address, 0x2a, 0b0);

    // digital power off
    write_i2c(dac_i2c_port, dac_address, 0x2, 0b0);

    // software reset on
    write_i2c(dac_i2c_port, dac_address, 0x0, 0b0);

    write_i2c(dac_i2c_port, dac_address, 0x4, 0b0);

    // clock2
    write_i2c(dac_i2c_port, dac_address, 0x6, 0b0);

    // audio i/f 1
    write_i2c(dac_i2c_port, dac_address, 0x10, 0b1011);

    // // dsd filter
    // write_i2c(dac_i2c_port, dac_address, 0x16, 0b110);

    uint8_t fir1 = 0;
    uint8_t fir2 = 0;
    uint8_t delsig = 0;

    if (freq <= 48000)
    {
        fir1 = 1;
        if(digital_filter = 0)
        {
            fir2 = 0;
        }
        else
        {
            fir2 = 3;
        }
        delsig = 17;
    }
    else if (freq <= 96000)
    {
        fir1 = 2;
        if(digital_filter = 0)
        {
            fir2 = 1;
        }
        else
        {
            fir2 = 4;
        }
        delsig = 17;
    }
    else if (freq <= 192000)
    {
        fir1 = 4;
        if(digital_filter = 0)
        {
            fir2 = 2;
        }
        else
        {
            fir2 = 5;
        }
        delsig = 17;
    }
    else
    {
        fir1 = 8;
        fir2 = 0x80;
        delsig = 17;
    }
    
    // fir filter 1
    write_i2c(dac_i2c_port, dac_address, 0x30, fir1);

    // fir filter 2
    write_i2c(dac_i2c_port, dac_address, 0x31, fir2);

    // delta sigma
    write_i2c(dac_i2c_port, dac_address, 0x40, delsig);

    // setting 5
    write_i2c(dac_i2c_port, dac_address, 0x60, 0b10110);

    // setting 6
    write_i2c(dac_i2c_port, dac_address, 0x61, 0b10110);

    // software reset off
    write_i2c(dac_i2c_port, dac_address, 0x0, 0b1);

    // digital power on
    write_i2c(dac_i2c_port, dac_address, 0x2, 0b1);

    // ram clear on
    write_i2c(dac_i2c_port, dac_address, 0x2f, 0b10000000);

    // ram clear off
    write_i2c(dac_i2c_port, dac_address, 0x2f, 0b0);

    // mute off
    write_i2c(dac_i2c_port, dac_address, 0x2a, 0b11);
}

bool bd34352::set_volume(uint8_t vol)
{
    uint8_t vol_value_send = vol_min - ((vol_min - vol_max) * vol / 100);

    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    write_i2c(dac_i2c_port, dac_address, 0x21, vol_value_send);
    write_i2c(dac_i2c_port, dac_address, 0x22, vol_value_send);
    return true;
}

uint8_t bd34352::get_digital_filter_num()
{
    return digital_filter_num;
}

string bd34352::get_digital_filter_strs(uint8_t filter_num)
{
    return digital_filter_strs[filter_num];
}

void bd34352::set_digital_filter(int filter_num)
{
    if (filter_num < digital_filter_num)
    {
        digital_filter = filter_num;
        set_bit_freq(32, sampling_rate);
    }
}

string bd34352::get_digital_filter_text_name()
{
    return digital_filter_text;
}

dac_type bd34352::get_dac()
{
    return dac_bd34352;
}