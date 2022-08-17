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

#include "general_i2s.h"

general_i2s::general_i2s()
{
    
}

void general_i2s::set_dac_address(uint8_t address)
{

}

void general_i2s::set_i2c_port(i2c_inst_t i2c_port)
{
    
}

void general_i2s::setup()
{
    
}

void general_i2s::mute()
{
    
}

void general_i2s::unmute()
{
    
}

void general_i2s::set_bit_freq(uint8_t bit, uint32_t freq)
{

}

bool general_i2s::set_volume(uint8_t vol)
{
    return false;
}

uint8_t general_i2s::get_digital_filter_num()
{
    return 0;
}

string general_i2s::get_digital_filter_strs(uint8_t filter_num)
{
    return "  No digital filter available";
}

void general_i2s::set_digital_filter(int filter_num)
{

}

string general_i2s::get_digital_filter_text_name()
{
    return "";
}

dac_type general_i2s::get_dac()
{
    return dac_general_i2s;
}