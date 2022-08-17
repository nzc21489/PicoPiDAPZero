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

#include "ak449x.h"
#include "pico_i2c.h"

ak449x::ak449x()
{
    
}

void ak449x::set_dac_address(uint8_t address)
{
    dac_address = address;
}

void ak449x::set_i2c_port(i2c_inst_t i2c_port)
{
    dac_i2c_port = i2c_port;
}

void ak449x::setup()
{
    for (uint8_t i = 0; i < 10; i++)
    {
        write_i2c(dac_i2c_port, dac_address, i, register_value[i]);
    }

    // reset ak449x
    register_value[0] &= 0b11111110;
    write_i2c(dac_i2c_port, dac_address, 0, register_value[0]);

    register_value[0] |= 0b00000001;
    write_i2c(dac_i2c_port, dac_address, 0, register_value[0]);
}

void ak449x::mute()
{
    // not implemented yet
}

void ak449x::unmute()
{
    // not implemented yet
}

void ak449x::set_bit_freq(uint8_t bit, uint32_t freq)
{

}

bool ak449x::set_volume(uint8_t vol)
{
    uint8_t vol_value_send = ((vol_max - vol_min) * vol / 100);

    if (vol == 0)
    {
        vol_value_send = vol_min;
    }

    register_value[3] = vol_value_send;
    register_value[4] = vol_value_send;

    write_i2c(dac_i2c_port, dac_address, 3, vol_value_send);
    write_i2c(dac_i2c_port, dac_address, 4, vol_value_send);
    return true;
}

uint8_t ak449x::get_digital_filter_num()
{
    return digital_filter_num;
}

string ak449x::get_digital_filter_strs(uint8_t filter_num)
{
    return digital_filter_strs[filter_num];
}

void ak449x::set_digital_filter(int filter_num)
{
    switch (filter_num)
    {
    case 0:
        register_value[1] &= 0b11011111;
        register_value[2] &= 0b11111110;
        register_value[5] &= 0b11111110;
        break;

    case 1:
        register_value[1] &= 0b11011111;
        register_value[2] |= 0b00000001;
        register_value[5] &= 0b11111110;
        break;

    case 2:
        register_value[1] |= 0b00100000;
        register_value[2] &= 0b11111110;
        register_value[5] &= 0b11111110;
        break;

    case 3:
        register_value[1] |= 0b00100000;
        register_value[2] |= 0b00000001;
        register_value[5] &= 0b11111110;
        break;

    case 4:
        register_value[1] &= 0b11011111;
        register_value[2] &= 0b11111110;
        register_value[5] |= 0b00000001;
        break;

    case 5:
        register_value[1] |= 0b00100000;
        register_value[2] &= 0b11111110;
        register_value[5] |= 0b00000001;
        break;
        
    default:
        break;
    }

    write_i2c(dac_i2c_port, dac_address, 1, register_value[1]);
    write_i2c(dac_i2c_port, dac_address, 2, register_value[2]);
    write_i2c(dac_i2c_port, dac_address, 5, register_value[5]);
}

string ak449x::get_digital_filter_text_name()
{
    return digital_filter_text;
}

dac_type ak449x::get_dac()
{
    return dac_ak449x;
}