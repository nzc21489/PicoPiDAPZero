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

#include "pcm1795.h"
#include "pico_i2c.h"

pcm1795::pcm1795()
{
    
}

void pcm1795::set_dac_address(uint8_t address)
{
    dac_address = address;
}

void pcm1795::set_i2c_port(i2c_inst_t i2c_port)
{
    dac_i2c_port = i2c_port;
}

void pcm1795::setup()
{
    sleep_ms(50);
    
    // set format I2S 32bit, Attenuation enable
    write_i2c(dac_i2c_port, dac_address, 18, 0b11000000);

    set_volume(0);
    write_i2c(dac_i2c_port, dac_address, 18, 0b11000000);
}

void pcm1795::mute()
{
    // not implemented yet
}

void pcm1795::unmute()
{
    // not implemented yet
}

void pcm1795::set_bit_freq(uint8_t bit, uint32_t freq)
{

}

bool pcm1795::set_volume(uint8_t vol)
{
    uint8_t vol_value_send = ((vol_max - vol_min) * vol / 100) + vol_min;

    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    write_i2c(dac_i2c_port, dac_address, 16, vol_value_send);
    write_i2c(dac_i2c_port, dac_address, 17, vol_value_send);
    write_i2c(dac_i2c_port, dac_address, 18, 0b11000000); // update volume
    return true;
}

uint8_t pcm1795::get_digital_filter_num()
{
    return digital_filter_num;
}

string pcm1795::get_digital_filter_strs(uint8_t filter_num)
{
    return digital_filter_strs[filter_num];
}

void pcm1795::set_digital_filter(int filter_num)
{
    if (filter_num < digital_filter_num)
    {
        write_i2c(dac_i2c_port, dac_address, 19, (digital_filter_nums[filter_num] << 1));
    }
}

string pcm1795::get_digital_filter_text_name()
{
    return digital_filter_text;
}

dac_type pcm1795::get_dac()
{
    return dac_pcm1795;
}